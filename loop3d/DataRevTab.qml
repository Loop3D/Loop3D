import QtQuick 2.14
import QtLocation 5.14
import QtPositioning 5.14
import QtQuick.Controls 2.14
import loop3d.eventmodel 1.0
import loop3d.observationmodel 1.0

Item {
    id: dmTab
    // Main area for data review tab
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
        ObservationModel {
            id: observationsModel
            observations: observationList
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
                        text: "Event " + eventID + "\t" + (type == 0 ? "Fault" : (type == 1 ? "Fold" : (type == 2 ? "Foliation" : (type == 0 ? "Discontinuity" : "Strata")))) + " \t" + name
                        font.family: mainWindow.defaultFontStyle
                        font.pixelSize: mainWindow.defaultFontSize
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (detailsView.currentIndex == index) {
                                detailsView.currentIndex = -1
                                observationView.selectedEntityId = -1
                                observationView.isEntityActive = true
                            } else {
                                detailsView.currentIndex = index
                                observationView.selectedEntityId = eventID
                                observationView.isEntityActive = isActive
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: visualisationDM
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#999999"
            // This section is a list of observations of the above selected event/feature with
            // eventId, position in (easting,northing,altitude) and appropriate info based on the
            // type of event selected (dip, dipdir, for fault), (dip,dipDir,dipPolarity,layername for strata), etc"
            Text {
                id: visualisationDMHeader
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.margins: mainWindow.myBorders
                text: "Observation List"
                height: 20
                font.bold: true
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignTop
            }

            ScrollView {
                id: observationView
                property int selectedEntityId: -1
                property bool isEntityActive: true
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: visualisationDMHeader.bottom
                clip: true
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ListView {
                    id: visualisationDMView
                    anchors.fill: parent
                    anchors.margins: mainWindow.myBorders
                    model: observationsModel
                    delegate: observationsDelegate
                    highlight: Rectangle { height: 28; color: "#8849ffff"; radius: 20 }
                    currentIndex: -1
                    highlightFollowsCurrentItem: true
                    highlightMoveDuration: 200
                    highlightMoveVelocity: -1
                }
            }
            Component {
                id: observationsDelegate
                Item {
                    id: observationComponent
                    property int componentHeight: 30
                    width: parent.width - mainWindow.myBorders
                    height: eventID == observationView.selectedEntityId ? 30 : 0
                    Text {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: mainWindow.myBorders
                        height: parent.height
                        verticalAlignment: Text.AlignVCenter
                        color: eventID == observationView.selectedEntityId ? "#000000" : "#00000000"
                        font.italic: observationView.isEntityActive ? false : true
                        text: eventID + "\t(" + Math.round(easting) + "," + Math.round(northing) + "," + Math.round(altitude) + ")\t"
                              + (type == 0 ? "Fault\t" + dip + "\t" + dipDir + "\t" + dipPolarity : "")
                              + (type == 1 ? "Fold\t (" + axisX + "," + axisY + "," + axisZ + ")" + "\t" + foliation + "\t" + whatIsFolded : "")
                              + (type == 2 ? "Foliation\t " + dip + "\t" + dipDir : "")
                              + (type == 3 ? "Discont\t " + dip + "\t" + dipDir : "")
                              + (type == 4 ? "Strata\t " + dip + "\t" + dipDir + "\t" + dipPolarity + "\t" + layer : "")
                        font.family: mainWindow.defaultFontStyle
                        font.pixelSize: mainWindow.defaultFontSize
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            visualisationDMView.currentIndex = index
                        }
                    }
                }
            }
        }
        Rectangle {
            // This section should contain a geological map of the region of interest
            // With overlayed observation points.  The points should be selectable and which in turn highlights that points feature and
            // that observation in the event log and observation list
            // There also needs to be a place to show the structural information of event (radial diagram of observations for example)
            // Also somewhere on this page there should be a stratigraphic column showing the colours and names of the strata
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
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: "(TO BE REPLACED) Map of Region of Interest\nAdd all observations as an overlay so they are selectable and toggled on/off\nHighlight selected event in event log and on map" +
                "\nNeed a place to show structural information of event, fault area\n of displacement, etc (see Map2Loop notebooks for examples of these diagrams)"
            }
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
