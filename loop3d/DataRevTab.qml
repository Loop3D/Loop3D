import QtQuick 2.14
import QtLocation 5.14
import QtPositioning 5.14
import QtQuick.Controls 2.14
import loop3d.eventmodel 1.0

Item {
    id: dmTab
    // Main area for data collection tab
    Rectangle {
        border.color: "#ffffff"
        border.width: 0
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#5151b0"
            }
            GradientStop {
                position: 1
                color: "#0f0f42"
            }
        }

        EventModel {
            id: eventsModel
            events: eventList
        }
        Rectangle {
            id: configDM
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#dddddd"

            Text {
                id: configDMHeader
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.margins: mainWindow.myBorders
                text: "Event Log"
                height: 20
                font.bold: true
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignTop
            }

            ScrollView {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: configDMHeader.bottom
                clip: true
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ListView {
                    id: detailsView
                    anchors.fill: parent
                    anchors.margins: mainWindow.myBorders
                    model: eventsModel
                    delegate: eventsDelegate
                    highlight: Rectangle { height: 28; color: "#8849ffff"; radius: 20 }
                    currentIndex: -1
                    highlightFollowsCurrentItem: true
                    highlightMoveDuration: 200
                    highlightMoveVelocity: -1
                }
            }
            Component {
                id: eventsDelegate
                Item {
                    width: parent.width - mainWindow.myBorders
                    height: 30

                    Text {
                        anchors.fill: parent
                        anchors.margins: mainWindow.myBorders
                        verticalAlignment: Text.AlignVCenter
                        color: isActive ? "#000000" : "#555555"
                        font.italic: isActive ? false : true
                        text: "Event " + eventID + "\t" + (type == 0 ? "Fault" : "") + " \t" + name
                        font.family: mainWindow.defaultFontStyle
                        font.pixelSize: mainWindow.defaultFontSize
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            detailsView.currentIndex = index
                        }
                    }
                }
            }
        }
//        Rectangle {
//            id: configDM
//            anchors.top: parent.top
//            anchors.left: parent.left
//            anchors.margins: mainWindow.myBorders
//            width: parent.width / 2 - mainWindow.myBorders - 1
//            height: parent.height / 2 - mainWindow.myBorders - 1
//            color: "#dddddd"
//            clip: true
//            Text {
//                anchors.fill: parent
//                verticalAlignment: Text.AlignVCenter
//                horizontalAlignment: Text.AlignHCenter
//                text: "List of Features (fault if a fault is selected, fold if ...,\n formation if ..., discontinuity if ...)"
//            }
//        }
        Rectangle {
            id: visualisationDM
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#999999"
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "Table of Points of the feature selected\n (position, dip, dipdir, etc)"
            }
        }
        Rectangle {
            Image {
                id: map
                source: project.filename + ".jpg"
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit

            }
            id: detailsDM
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            color: "#bbbbbb"
        }
        Button {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 10
            text: "Reload"
            onPressed: {
                map.source = ""
                map.source = project.filename + ".jpg"
            }

        }
    }
}
