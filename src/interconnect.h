/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include <BluezQt/Manager>
#include <QQmlEngine>
#include <QTimer>
#include <QSet>

#include "frame.h"

class Track;

class InterConnect: public BluezQt::Manager
{
    Q_OBJECT
    Q_PROPERTY(QStringList devices READ devices NOTIFY devicesChanged)
    Q_PROPERTY(QVariantList tracks READ tracks NOTIFY tracksChanged)

 public:
    ~InterConnect();

    static QObject* instance(QQmlEngine *e, QJSEngine *js);

    QStringList devices() const;
    QVariantList tracks() const;

 signals:
    void devicesChanged();
    void tracksChanged();

 private:
    InterConnect(QObject *parent = nullptr);
    void initialized(BluezQt::InitManagerJob *job);
    void scan(BluezQt::AdapterPtr adapter);
    void autoConnect(BluezQt::DevicePtr device);
    void disconnect(BluezQt::DevicePtr device);
    void readFrame(const QString &device, const Frame &frame);
    void checkPing();

    QString mSppUuid;
    QStringList mDevices;
    QHash<QString, Track*> mTracks;
    QTimer mPingTimer;
    QStringList mDevicesByAddress;
    QSet<QString> mAliveDevices, mDeadDevices;
};

#endif
