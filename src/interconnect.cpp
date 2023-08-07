/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "interconnect.h"

#include <QDebug>
#include <QDateTime>
#include <BluezQt/InitManagerJob>
#include <BluezQt/Adapter>
#include <BluezQt/Device>
#include <BluezQt/PendingCall>

#include "spp.h"

static InterConnect *singleton = nullptr;
QObject* InterConnect::instance(QQmlEngine *e, QJSEngine *js)
{
    if (!singleton) {
        singleton = new InterConnect(e);
    }
    return singleton;
}

InterConnect::InterConnect(QObject *parent)
    : BluezQt::Manager(parent)
{
    BluezQt::InitManagerJob *job = init();
    job->start();
    connect(job, &BluezQt::InitManagerJob::result,
            this, &InterConnect::initialized);
    mPingTimer.setInterval(3000);
    connect(&mPingTimer, &QTimer::timeout, this, &InterConnect::checkPing);
}

InterConnect::~InterConnect()
{
}

void InterConnect::initialized(BluezQt::InitManagerJob *job)
{
    job->deleteLater();

    Spp *spp = new Spp(QString(), this);
    connect(spp, &Spp::frameAvailable,
            this, &InterConnect::readFrame);
    connect(spp, &Spp::connected,
            [this] (const QString &device, const QString &name) {
                qDebug() << "profile connected" << device << name;
                mDevices.append(name);
                emit devicesChanged();
                mDevicesByAddress.append(device);
                mAliveDevices.insert(device);
                mPingTimer.start();
            });
    connect(spp, &Spp::disconnected,
            [this] (const QString &device, const QString &name) {
                qDebug() << "profile disconnected" << device << name;
                if (mDevicesByAddress.contains(device)) {
                    mDevicesByAddress.removeAll(device);
                    mDevices.removeAll(name);
                    emit devicesChanged();
                    for (const QString &key : mTracks.keys()) {
                        if (key.startsWith(device)) {
                            mTracks.remove(key);
                        }
                    }
                    emit tracksChanged();
                    mDeadDevices.insert(device);
                }
            });
    registerProfile(spp);
    mSppUuid = spp->uuid();

    BluezQt::AdapterPtr adapter = usableAdapter();
    if (!adapter) {
        qDebug() << "no powered adapter";
        connect(this, &BluezQt::Manager::adapterAdded,
                [this] (BluezQt::AdapterPtr adapter) {
                    if (adapter->isPowered()) {
                        qDebug() << "found an adapter, starting discovery.";
                        scan(adapter);
                    }
                });
        connect(this, &BluezQt::Manager::adapterChanged,
                [this] (BluezQt::AdapterPtr adapter) {
                    if (adapter->isPowered()) {
                        qDebug() << "adapter is powered, starting discovery.";
                        scan(adapter);
                    }
                });
    } else {
        scan(adapter);
    }
}

void InterConnect::scan(BluezQt::AdapterPtr adapter)
{
    connect(adapter.data(), &BluezQt::Adapter::deviceAdded,
            this, &InterConnect::autoConnect);
    connect(adapter.data(), &BluezQt::Adapter::deviceRemoved,
            this, &InterConnect::disconnect);
    adapter->startDiscovery();
    for (BluezQt::DevicePtr device : adapter->devices()) {
        autoConnect(device);
    }
}

void InterConnect::autoConnect(BluezQt::DevicePtr device)
{
    qDebug() << device->address() << device->name();
    qDebug() << device->uuids();
    if (device->uuids().contains(mSppUuid) || device->name() == "ESP train") {
        BluezQt::PendingCall *call = device->connectProfile(mSppUuid);
        connect(call, &BluezQt::PendingCall::finished,
                [device] (BluezQt::PendingCall *call) {
                    if (call->error() != BluezQt::PendingCall::NoError) {
                        qWarning() << device->name() << "auto connect error:" << call->errorText();
                    }
                    //call->deleteLater();
                    qDebug() << "connected to" << device->name();
                });
    }
}

void InterConnect::disconnect(BluezQt::DevicePtr device)
{
    if (mDevicesByAddress.contains(device->address())) {
        qDebug() << "request disconnection" << device->address() << device->name();
        BluezQt::PendingCall *call = device->disconnectProfile(mSppUuid);
        connect(call, &BluezQt::PendingCall::finished,
                [device] (BluezQt::PendingCall *call) {
                    if (call->error() != BluezQt::PendingCall::NoError) {
                        qWarning() << device->name() << "disconnection error:" << call->errorText();
                    }
                    //call->deleteLater();
                    qDebug() << "disconnected from" << device->name();
                });
    }
}

void InterConnect::checkPing()
{
    qDebug() << "checking ping at" << QDateTime::currentDateTime();
    for (const QString &device : mDevicesByAddress) {
        qDebug() << "testing device" << device << "from" << mAliveDevices;
        QSet<QString>::Iterator it = mAliveDevices.find(device);
        if (it == mAliveDevices.end()) {
            disconnect(deviceForAddress(device));
        } else {
            mAliveDevices.erase(it);
        }
    }
    QSet<QString>::Iterator it = mDeadDevices.begin();
    while (it != mDeadDevices.end()) {
        qDebug() << "trying to reconnect device" << *it;
        BluezQt::DevicePtr device = deviceForAddress(*it);
        if (device) autoConnect(device);
        it = mDeadDevices.erase(it);
    }
}

void InterConnect::readFrame(const QString &device, const Frame &frame)
{
    Spp *spp = qobject_cast<Spp*>(sender());
    if (!spp)
        return;

    switch (frame.type()) {
    case Frame::PING: {
        qDebug() << "received a ping frame, preparing response" << device;
        spp->send(device, frame.pingResponse());
        mAliveDevices.insert(device);
        return;
    }
    case Frame::CAPABILITIES: {
        for (const Track::Definition &definition : frame.trackDefinitions()) {
            Track *track = new Track(definition, this);
            const QString key = device
                + QString::fromLatin1("::") + QString::number(track->id());
            if (mTracks.contains(key)) {
                qWarning() << "unable to redefine track" << key;
                delete track;
            } else {
                qDebug() << "inserting a new track" << key << track->label();
                mTracks.insert(key, track);
                connect(track, &Track::acquireRequest,
                        [spp, device, track] () {
                            spp->send(device, Frame::acquireFrame(track->id()));
                        });
                connect(track, &Track::releaseRequest,
                        [spp, device, track] () {
                            spp->send(device, Frame::releaseFrame(track->id()));
                        });
                connect(track, &Track::speedRequest,
                        [spp, device, track] (int speed) {
                            spp->send(device, Frame::speedFrame(track->id(), speed));
                        });
            }
        }
        emit tracksChanged();
        return;
    }
    case Frame::TRACK_STATE: {
        int id;
        const Track::State state = frame.trackState(&id);
        Track *tr = track(device, id);
        if (!tr) {
            qWarning() << "unknown track" << device << id;
        } else {
            qDebug() << "updating state" << device << id;
            tr->setState(state);
        }
        return;
    }
    case Frame::ACQUIRE_ACK: {
        int id;
        bool ack = frame.ack(&id);
        Track *tr = track(device, id);
        if (!tr) {
            qWarning() << "unknown track" << device << id;
        } else {
            qDebug() << "acquire ack" << device << id << ack;
            if (ack)
                tr->setLinked(true);
        }
        return;
    }
    case Frame::RELEASE_ACK: {
        int id;
        bool ack = frame.ack(&id);
        Track *tr = track(device, id);
        if (!tr) {
            qWarning() << "unknown track" << device << id;
        } else {
            qDebug() << "release ack" << device << id << ack;
            if (ack)
                tr->setLinked(false);
        }
        return;
    }
    default:
        return;
    }
}

QStringList InterConnect::devices() const
{
    return mDevices;
}

QVariantList InterConnect::tracks() const
{
    QVariantList list;
    for (Track *track : mTracks) {
        list.append(QVariant::fromValue(track));
    }
    return list;
}

Track* InterConnect::track(const QString &device, int id) const
{
    const QString key = device
        + QString::fromLatin1("::") + QString::number(id);
    if (!mTracks.contains(key)) {
        return nullptr;
    } else {
        return mTracks[key];
    }
}
