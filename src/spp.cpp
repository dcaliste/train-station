/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "spp.h"

#include <QDBusObjectPath>
#include <BluezQt/Device>

Spp::Spp(const QString &uuid, QObject *parent)
    : BluezQt::Profile(parent)
    , mUuid(uuid.isEmpty() ? QString::fromLatin1("00001101-0000-1000-8000-00805F9B34FB") : uuid)
{
    setAutoConnect(false);
    setChannel(0);
    setLocalRole(BluezQt::Profile::ClientRole);
    setName(QString::fromLatin1("Train station Serial Port Profile"));
    setRequireAuthentication(false);
    setVersion(1);
}

Spp::~Spp()
{
}

QString Spp::uuid() const
{
    return mUuid;
}

QDBusObjectPath Spp::objectPath() const
{
    return QDBusObjectPath("/bluetooth/profile/serial_port");
}

void Spp::newConnection(BluezQt::DevicePtr device,
                        const QDBusUnixFileDescriptor &fd,
                        const QVariantMap &properties,
                        const BluezQt::Request<> &request)
{
    if (mSockets.contains(device->address())) {
        qWarning() << "device already connected" << device->address();
        request.cancel();
        return;
    }
    QSharedPointer<QLocalSocket> socket = createSocket(fd);
    if (!socket->isValid()) {
        request.cancel();
        return;
    }
    qDebug() << "new connection to" << device->address() << device->name();
    socket->setProperty("device", device->address());
    connect(socket.data(), &QIODevice::readyRead,
            this, &Spp::dataAvailable);
    mSockets.insert(device->address(), socket);
    emit connected(device->address(), device->name());
    request.accept();
}

void Spp::release()
{
    qDebug() << "releasing";
}

void Spp::requestDisconnection(BluezQt::DevicePtr device,
                               const BluezQt::Request<> &request)
{
    qDebug() << "disconnecting profile" << device->address() << device->name();
    mSockets.remove(device->address());
    emit disconnected(device->address(), device->name());
    request.accept();
}

void Spp::dataAvailable()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket*>(sender());
    emit frameAvailable(socket->property("device").toString(),
                        Frame(socket->readAll()));
}

void Spp::send(const QString &device, const QByteArray &data) const
{
    QHash<QString, QSharedPointer<QLocalSocket>>::ConstIterator it = mSockets.find(device);
    if (it == mSockets.constEnd()) {
        qWarning() << "Unknown device" << device;
        return;
    }

    qDebug() << "sending data to" << device << data;
    const char *pt = data.constData();
    qint64 len = data.length();
    do {
        qint64 part = (*it)->write(pt, len);
        if (part < 0) {
            qWarning() << "Error sending" << data;
            return;
        }
        pt += part;
        len -= part;
    } while (len > 0);
    qDebug() << "data sent to" << device;
}
