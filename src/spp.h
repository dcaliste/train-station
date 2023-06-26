/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SPP_H
#define SPP_H

#include <BluezQt/Profile>
#include <QSharedPointer>
#include <QLocalSocket>

#include "frame.h"

class Spp: public BluezQt::Profile
{
    Q_OBJECT
 public:
    Spp(const QString &uuid = QString(), QObject *parent = nullptr);
    ~Spp();

    QString uuid() const override;
    QDBusObjectPath objectPath() const override;

    void newConnection(BluezQt::DevicePtr device,
                       const QDBusUnixFileDescriptor &fd,
                       const QVariantMap &properties,
                       const BluezQt::Request<> &request) override;
    void requestDisconnection(BluezQt::DevicePtr device,
                              const BluezQt::Request<> &request) override;

 signals:
    void connected(const QString &device, const QString &name);
    void disconnected(const QString &device, const QString &name);
    void frameAvailable(const QString &device, const Frame &frame);

 private:
    void dataAvailable();
    
    QString mUuid;
    QHash<QString, QSharedPointer<QLocalSocket>> mSockets;
};

#endif
