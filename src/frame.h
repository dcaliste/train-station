/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef FRAME_H
#define FRAME_H

#include <QByteArray>
#include <QDataStream>

#include "track.h"

class Frame
{
public:
    enum Types {
                UNSUPPORTED,
                PING,
                CAPABILITIES,
                TRACK_STATE
    };

    Frame(const QByteArray &data);
    ~Frame();

    Types type() const;
    QList<Track::Definition> trackDefinitions() const;
    Track::State trackState(int *id) const;
    QByteArray pingResponse() const;

private:
    void read(const QByteArray &data);
    void readPing(QDataStream &stream);
    void readCapabilities(QDataStream &stream);
    void readTrackState(QDataStream &stream);

    Types mType = UNSUPPORTED;
    quint64 mPingCount = 0;
    QList<Track::Definition> mTrackDefinitions;
    Track::State mTrackState;
    quint32 mTrackId = 0;
};

#endif
