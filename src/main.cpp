/*
 * This file is part of train-station.
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QtQuick>

#include <sailfishapp.h>

#include "interconnect.h"
#include "track.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setOrganizationName("Train");
    app->setApplicationName("Station");

    qmlRegisterUncreatableType<Track>("Train.Station", 1, 0, "Track",
                                      "Track can be obtained from InterConnect.");
    qmlRegisterSingletonType<InterConnect>("Train.Station", 1, 0, "InterConnect",
                                           InterConnect::instance);

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setSource(SailfishApp::pathToMainQml());
    view->show();
    return app->exec();
}
