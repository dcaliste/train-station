/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "track.h"

#include <QDebug>
#include <QtEndian>

Track::Track(QObject *parent)
    : QObject(parent)
{
}

Track::Track(const Definition &definition, QObject *parent)
    : QObject(parent)
    , mDefinition(definition)
{
}

Track::~Track()
{
}

int Track::id() const
{
    return mDefinition.mId;
}

QString Track::label() const
{
    return mDefinition.mLabel;
}

Track::Direction Track::direction() const
{
    return mState.mDirection;
}

float Track::speed() const
{
    return mState.mSpeed;
}

int Track::count() const
{
    return mState.mCount;
}

Track::Position Track::position() const
{
    return mState.mPosition;
}

void Track::setState(const State &state)
{
    State old = mState;

    mState = state;
    if (old.mDirection != mState.mDirection) {
        qDebug() << "direction:" << mState.mDirection;
        emit directionChanged();
    }
    if (old.mSpeed != mState.mSpeed) {
        qDebug() << "speed:" << mState.mSpeed;
        emit speedChanged();
    }
    if (old.mCount != mState.mCount) {
        qDebug() << "count:" << mState.mCount;
        emit countChanged();
    }
    if (old.mPosition != mState.mPosition) {
        qDebug() << "position:" << mState.mPosition;
        emit positionChanged();
    }
}

Track::Definition::Definition()
{
}

Track::Definition::Definition(QDataStream &in)
{
    quint32 id = 0;
    in >> id;
    mId = qFromBigEndian<quint32>(id);
    quint32 ln = 0;
    in >> ln;
    ln = qFromBigEndian<quint32>(ln);
    char *label = new char[ln + 1];
    in.readRawData(label, ln + 1);
    mLabel = QString::fromUtf8(label);
    delete[] label;
}

Track::State::State()
{
}

Track::State::State(QDataStream &in)
{
    qint32 isForward, isBackward;
    in >> isForward >> isBackward;
    mDirection = Track::IDLE;
    if (qFromBigEndian<qint32>(isForward)) {
        mDirection = Track::FORWARD;
    }
    if (qFromBigEndian<qint32>(isBackward)) {
        mDirection = Track::BACKWARD;
    }
    qint32 speed;
    in >> speed;
    mSpeed = float(qFromBigEndian<qint32>(speed)) / 4096.f;
    quint32 count;
    in >> count;
    mCount = qFromBigEndian<quint32>(count);
    qint32 state;
    in >> state;
    state = qFromBigEndian<qint32>(state);
    mPosition = Track::SOMEWHERE;
    if (state == 1) {
        mPosition = Track::APPROACHING;
    } else if (state == 2) {
        mPosition = Track::PASSING_BY;
    } else if (state == 3) {
        mPosition = Track::STOPPING;
    } else if (state == 4) {
        mPosition = Track::IN_STATION;
    } else if (state == 5) {
        mPosition = Track::LEAVING;
    }
}
