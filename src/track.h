/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TRACK_H
#define TRACK_H

#include <QObject>
#include <QDataStream>

class Track: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label CONSTANT);
    Q_PROPERTY(Direction direction READ direction NOTIFY directionChanged);
    Q_PROPERTY(float speed READ speed NOTIFY speedChanged);
    Q_PROPERTY(int count READ count NOTIFY countChanged);
    Q_PROPERTY(Position position READ position NOTIFY positionChanged);

 public:
    enum Direction
        {
         FORWARD,
         BACKWARD,
         IDLE
        };
    Q_ENUM(Direction);
    
    enum Position
        {
         SOMEWHERE,
         APPROACHING,
         PASSING_BY,
         STOPPING,
         IN_STATION,
         LEAVING
        };
    Q_ENUM(Position);

    struct Definition
    {
    public:
        Definition();
        Definition(QDataStream &in);
    private:
        friend class Track;
        int mId = -1;
        QString mLabel;
    };

    struct State
    {
    public:
        State();
        State(QDataStream &in);
    private:
        friend class Track;
        Direction mDirection = Track::IDLE;
        float mSpeed = 0.;
        int mCount = 0;
        Position mPosition = Track::SOMEWHERE;
        
    };

    Track(QObject *parent = nullptr);
    Track(const Definition &definition, QObject *parent = nullptr);
    ~Track();

    int id() const;
    QString label() const;
    Direction direction() const;
    float speed() const;
    int count() const;
    Position position() const;

    void setState(const State &state);

 signals:
    void directionChanged();
    void speedChanged();
    void countChanged();
    void positionChanged();

 private:
    Definition mDefinition;
    State mState;
};
Q_DECLARE_METATYPE(Track*);

#endif
