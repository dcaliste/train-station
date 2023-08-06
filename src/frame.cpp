/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "frame.h"

#include <QDebug>
#include <QtEndian>

Frame::Frame(const QByteArray &data)
{
    read(data);
}

Frame::~Frame()
{
}

Frame::Types Frame::type() const
{
    return mType;
}

void Frame::read(const QByteArray &data)
{
    QDataStream stream(data);

    quint32 type = 0;
    stream >> type;
    switch (qFromBigEndian<quint32>(type)) {
    case PING:
        mType = PING;
        readPing(stream);
        return;
    case CAPABILITIES:
        mType = CAPABILITIES;
        readCapabilities(stream);
        return;
    case TRACK_STATE:
        mType = TRACK_STATE;
        readTrackState(stream);
        return;
    default:
        qWarning() << "unknown type from frame" << type;
        mType = UNSUPPORTED;
    }
}

void Frame::readPing(QDataStream &stream)
{
    quint64 count;
    stream >> count;
    mPingCount = qFromBigEndian<quint64>(count);
}

void Frame::readCapabilities(QDataStream &stream)
{
    quint32 nTracks;
    stream >> nTracks;
    for (quint32 i = 0; i < qFromBigEndian<quint32>(nTracks); i++) {
        mTrackDefinitions.append(Track::Definition(stream));
    }
}

void Frame::readTrackState(QDataStream &stream)
{
    quint32 id;
    stream >> id;
    mTrackId = qFromBigEndian<quint32>(id);
    mTrackState = Track::State(stream);
}

QList<Track::Definition> Frame::trackDefinitions() const
{
    return mType == CAPABILITIES ? mTrackDefinitions : QList<Track::Definition>();
}

Track::State Frame::trackState(int *id) const
{
    if (mType == TRACK_STATE) {
        *id = mTrackId;
        return mTrackState;
    } else {
        *id = -1;
        return Track::State();
    }
}

QByteArray Frame::pingResponse() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << qToBigEndian<quint32>(quint32(PING)) << qToBigEndian<quint64>(mPingCount);

    return data;
}
