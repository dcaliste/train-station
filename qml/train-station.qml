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
            allowedOrientations: Orientation.Landscape
            enabled: InterConnect.operational
            Column {
                width: parent.width / 3
                anchors.right: parent.right
                anchors.rightMargin: Theme.horizontalPageMargin
                Item {
                    id: header
                    width: parent.width
                    height: btIcon.height
                    Icon {
                        id: btIcon
                        source: "image://theme/icon-m-bluetooth"
                        highlighted: true
                        enabled: InterConnect.bluetoothOperational
                        opacity: enabled ? 1. : Theme.opacityLow
                    }
                    Label {
                        color: Theme.highlightColor
                        width: parent.width - btIcon.width - anchors.leftMargin - anchors.rightMargin
                        anchors.left: btIcon.right
                        anchors.leftMargin: Theme.paddingMedium
                        anchors.verticalCenter: btIcon.verticalCenter
                        text: InterConnect.devices.length
                            ? InterConnect.devices.length + " connected device(s)"
                            : "no connected device"
                    }
                }
                Repeater {
                    model: InterConnect.tracks
                    delegate: Item {
                        width: parent.width
                        height: (Screen.width - header.height) / InterConnect.tracks.length
                        Icon {
                            id: backward
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            source: "image://theme/icon-splus-left"
                            highlighted: modelData.direction == Track.BACKWARD
                        }
                        Slider {
                            enabled: false
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: backward.right
                            anchors.right: forward.left
                            label: modelData.label + " | " + modelData.count + " passage(s)"
                            value: modelData.speed
                        }
                        Icon {
                            id: forward
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            source: "image://theme/icon-splus-right"
                            highlighted: modelData.direction == Track.FORWARD
                        }
                    }
                }
                InfoLabel {
                    x: 0
                    height: Screen.width - header.height
                    verticalAlignment: Text.AlignVCenter
                    visible: InterConnect.tracks.length == 0
                    text: "no tracks"
                }
            }
        }
    }
}
