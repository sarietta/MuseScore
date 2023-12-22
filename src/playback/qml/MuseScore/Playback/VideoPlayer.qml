/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.15
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import QtMultimedia 5.12

import MuseScore.Playback 1.0
import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

StyledDialogView {
  id: root

  contentWidth: content.implicitWidth
  contentHeight: 600
  margins: 16

  modal: false
  resizable: true
  alwaysOnTop: model.alwaysOnTop

  QtObject {
    id: privateProperties
    
    readonly property int sideMargin: 36
    readonly property int buttonsMargin: 24
  }
  
  VideoPlayerModel {
    id: model

    onMusicPlaybackStarted: {
      player.seek(model.currentMusicPositionInMilliseconds())
      player.play()
    }

    onMusicPlaybackPaused: {
      player.pause()
    }

    onMusicPlaybackRewind: {
      player.seek(0)
    }

    onMusicPlaybackPositionUpdated: {
      player.seek(model.currentMusicPositionInMilliseconds())
    }

    onAlwaysOnTopChanged: {
      console.log("Always on top: " + model.alwaysOnTop)
      root.alwaysOnTop = model.alwaysOnTop
    }
  }

  Component.onCompleted: {
    model.load()
  }

  FileDialog {
    id: fileDialog
    title: "Please choose a file"
    folder: shortcuts.home
    selectMultiple: false
    nameFilters: ["Video Files (*.mp4 *.mkv *.avi)"]
    onAccepted: {
      player.source = fileDialog.fileUrl
    }
    onRejected: {
    }
  }
  
  ColumnLayout {
    id: content
    anchors.fill: parent
    spacing: 20
    Layout.alignment: Qt.AlignCenter

    Rectangle {
      color: "black"
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.leftMargin: privateProperties.buttonsMargin
      Layout.rightMargin: privateProperties.buttonsMargin
      Layout.bottomMargin: privateProperties.buttonsMargin

      MediaPlayer {
        id: player

        autoLoad: true
        source: "file:///Users/sarietta/tmp/score/spring.mp4"
        onPlaying: {
          model.handleVideoOnPlaying()
        }
        onPaused: {
          model.handleVideoOnPaused()
        }

        onStatusChanged: {
          if (player.status == MediaPlayer.Loaded) {
            model.setVideoFrameRate(player.metaData.videoFrameRate)
          }
        }
      }
      
      VideoOutput {
        id: videoOutput
        anchors.fill: parent
        source: player
      }
    }

    RowLayout {
      ColumnLayout {
        RowLayout {
          ButtonBox {
            Layout.fillWidth: true
            Layout.leftMargin: privateProperties.buttonsMargin
            Layout.rightMargin: privateProperties.buttonsMargin
            Layout.bottomMargin: privateProperties.buttonsMargin

            FlatButton {
              text: qsTrc("global", "Play")
              buttonRole: ButtonBoxModel.CustomRole
              buttonId: ButtonBoxModel.CustomButton + 1
              
              onClicked: {
                player.seek(model.currentMusicPositionInMilliseconds())
                player.play()
              }
            }

            FlatButton {
              text: qsTrc("global", "Pause")
              buttonRole: ButtonBoxModel.CustomRole
              buttonId: ButtonBoxModel.CustomButton + 2
              
              onClicked: {
                player.pause()
              }
            }

            FlatButton {
              text: qsTrc("global", "Restart")
              buttonRole: ButtonBoxModel.CustomRole
              buttonId: ButtonBoxModel.CustomButton + 3
              
              onClicked: {
                model.rewindMusicPlayback()
                player.seek(0)
              }
            }
          }
        }
      }

      RowLayout {
        ButtonBox {
          Layout.fillWidth: true
          Layout.leftMargin: privateProperties.buttonsMargin
          Layout.rightMargin: privateProperties.buttonsMargin
          Layout.bottomMargin: privateProperties.buttonsMargin
          
          FlatButton {
            text: qsTrc("global", "Open File")
            buttonRole: ButtonBoxModel.CustomRole
            buttonId: ButtonBoxModel.CustomButton + 1
            
            onClicked: {
              fileDialog.open()
            }
          }

          FlatButton {
            text: model.alwaysOnTop ? qsTrc("global", "Unpin Window") : qsTrc("global", "Pin Window")
            buttonRole: ButtonBoxModel.CustomRole
            buttonId: ButtonBoxModel.CustomButton + 2
            
            onClicked: {
              model.alwaysOnTop = !model.alwaysOnTop
            }
          }

        }
      }
    }
  }
}
