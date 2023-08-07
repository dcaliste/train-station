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
    cover: cover
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
            SilicaListView {
                id: trackList
                anchors.fill: parent
                header: Item {
                    x: trackList.width - width
                    width: trackList.width / 3
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
                model: InterConnect.tracks
                delegate: ListItem {
                    readonly property bool edited: slider.track == modelData
                    x: trackList.width - width
                    width: trackList.width / 3
                    contentHeight: (Screen.width - trackList.headerItem.height) / 4
                    menu: ContextMenu {
                        width: trackList.width / 3
                        x: 0
                        MenuItem {
                            text: modelData.linked ? "Release track control" : "Control track"
                            onClicked: {
                                if (modelData.linked) {
                                    modelData.release()
                                } else {
                                    modelData.acquire()
                                }
                            }
                        }
                    }
                    onClicked: {
                        if (modelData.linked) {
                            slider.track = edited ? null : modelData
                        }
                    }
                    Icon {
                        anchors.top: parent.top
                        anchors.topMargin: Theme.paddingSmall / 2
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "image://theme/icon-s-edit"
                        visible: modelData.linked
                        highlighted: parent.highlighted || edited
                    }
                    Icon {
                        id: backward
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingSmall
                        source: "image://theme/icon-splus-left"
                        highlighted: modelData.direction == Track.BACKWARD
                    }
                    Slider {
                        enabled: false
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingSmall
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.horizontalPageMargin
                        leftMargin: backward.width + Theme.paddingSmall
                        rightMargin: forward.width + Theme.paddingSmall
                        label: modelData.label + (modelData.capabilities & Track.POSITIONING ? " | " + modelData.count + " passage(s)" : "")
                        value: modelData.speed
                    }
                    Icon {
                        id: forward
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.horizontalPageMargin
                        source: "image://theme/icon-splus-right"
                        highlighted: modelData.direction == Track.FORWARD
                    }
                }
            }
            InfoLabel {
                x: parent.width - width
                width: parent.width / 3
                height: Screen.width
                verticalAlignment: Text.AlignVCenter
                visible: trackList.model.length == 0
                text: "no tracks"
            }
            Slider {
                id: slider

                property QtObject track
                onTrackChanged: {
                    if (track) {
                        value = track.direction == Track.FORWARD ? track.speed : -track.speed
                    }
                }

                anchors.bottom: parent.bottom
                width: 2 * parent.width / 3
                enabled: track !== null
                opacity: enabled ? 1. : Theme.opacityLow
                label: enabled ? track.label : "no controlled track"
                minimumValue: -1.
                valueText: {
                    if (Math.abs(sliderValue) > 0.05) {
                        return Math.round(sliderValue * 100.) + " %"
                    } else {
                        return "stopped"
                    }
                }
                onValueChanged: {
                    if (track) {
                        track.requestSpeed(value)
                    }
                }

                Binding {
                    target: slider._progressBarItem
                    property: "width"
                    value: 0.5 * ((slider.sliderValue < 0 ? 2. : 1.) * slider._progressBarItem.height + Math.abs(slider.sliderValue) * (slider._backgroundItem.width - slider._progressBarItem.height))
                }
                Binding {
                    target: slider._progressBarItem
                    property: "x"
                    value: slider._backgroundItem.x + 0.5 * slider._backgroundItem.width - (slider.sliderValue < 0 ? (slider._progressBarItem.width - 0.5 * slider._progressBarItem.height) : 0.)
                }
                Binding {
                    target: slider._progressBarItem
                    property: "visible"
                    value: slider.sliderValue != 0.
                }
            }
        }
    }

    Component {
        id: cover
        CoverBackground {
            id: coverBackground
            Column {
                width: parent.width
                Repeater {
                    model: InterConnect.tracks
                    delegate: Item {
                        width: parent.width
                        height: coverBackground.height / InterConnect.tracks.length
                        Slider {
                            enabled: false
                            width: parent.width
                            anchors.verticalCenter: parent.verticalCenter
                            label: modelData.label + (modelData.capabilities & Track.POSITIONING ? " | " + modelData.count + " passage(s)" : "")
                            value: modelData.speed
                        }
                    }
                }
            }
            InfoLabel {
                x: 0
                y: (parent.height - height) / 2.
                visible: InterConnect.tracks.length == 0
                text: "no tracks"
            }
        }
    }
}
