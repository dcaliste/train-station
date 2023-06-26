/*
 * This file is part of train-station
 *
 * SPDX-FileCopyrightText: 2023 Damien Caliste
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * harbour-todolist is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * harbour-todolist is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import Sailfish.Silica 1.0
import Train.Station 1.0
import MeeGo.Connman 0.2

ApplicationWindow
{
    initialPage: mainPage
    allowedOrientations: Orientation.Landscape

    Connections {
        target: Qt.application
        onAboutToQuit: btTechonology.powered = btTechonology.poweredOnStart
    }

    TechnologyModel {
        id: btTechonology
        property bool poweredOnStart: false
        name: "bluetooth"
        onAvailableChanged: {
            if (available) {
                poweredOnStart = powered
                powered = true
            }
        }
    }

    Component {
        id: mainPage
        Page {
            Column {
                width: parent.width
                Label {
                    text: "operational: " + InterConnect.operational
                }
                Label {
                    text: "BT operational: " + InterConnect.bluetoothOperational
                }
                Label {
                    text: "blocked: " + InterConnect.bluetoothBlocked
                }
                Label {
                    text: "nb connected devices: " + InterConnect.devices.length
                }
                Label {
                    text: "nb tracks: " + InterConnect.tracks.length
                }
            }
        }
    }
}
