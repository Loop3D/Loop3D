import QtQuick 2.14
import QtLocation 5.14
import QtPositioning 5.14
import QtQuick.Controls 2.14
import loop3d.eventmodel 1.0
import loop3d.observationmodel 1.0

Item {
    id: dmTab
    // Main area for data review tab
    function reloadMap() {
        map.source = ""
        map.source = project.filename === "" ? "" : project.filename + ".png"
    }

    function refreshModels() {
        eventsModel.refreshModel()
        observationsModel.refreshModel()
    }

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
                    width: configDM.width - mainWindow.myBorders
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
                                observationView.selectedEntityType = -1
                                observationView.isEntityActive = true
                            } else {
                                detailsView.currentIndex = index
                                observationView.selectedEntityId = eventID
                                observationView.selectedEntityType = type
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
                id: visualisationDMEventHeader
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.margins: mainWindow.myBorders
                text: detailsView.currentIndex >= 0 ? eventsModel.dataIndexed(detailsView.currentIndex,"name") + " Summary\n" : "Select an event above"
                height: mainWindow.defaultFontSize
                font.bold: true
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignTop
            }
            Text {
                property int type: detailsView.currentIndex >= 0 ? eventsModel.dataIndexed(detailsView.currentIndex,"type"):""
                property string typeStr: type == 0 ? "Fault" : (type == 1 ? "Fold" : (type == 2 ? "Foliation"
                                          : (type == 3 ? "Discontinuity" : "Strata")))
                property real minAge: parseFloat(eventsModel.dataIndexed(detailsView.currentIndex,"minAge")).toFixed(1)
                property real maxAge: parseFloat(eventsModel.dataIndexed(detailsView.currentIndex,"maxAge")).toFixed(1)
                id: visualisationDMEventDetails
                anchors.top: visualisationDMEventHeader.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.margins: mainWindow.myBorders
                visible: detailsView.currentIndex >= 0
                text: "ID: " + eventsModel.dataIndexed(detailsView.currentIndex,"eventID") + "\t"
                    + "Age Range: " + minAge + " - " + maxAge + "\t"
                height: mainWindow.defaultFontSize
                font.bold: false
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                verticalAlignment: Text.AlignTop
            }
            Text {
                id: visualisationDMHeader
                visible: detailsView.currentIndex >= 0
                anchors.top: visualisationDMEventDetails.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.margins: mainWindow.myBorders
                text: "Observation List\n"
                height: mainWindow.defaultFontSize
                font.bold: true
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignTop
            }
            Text {
                id: visualisationDMHeadings
                anchors.top: visualisationDMHeader.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.margins: mainWindow.myBorders
                text: (observationView.selectedEntityType == 0 ? "(East/North/Alt)\tDipDir\tDip\tDipPol\tDispl\tVal" : "")
                    + (observationView.selectedEntityType == 1 ? "(East/North/Alt)\t(AxisX/AxisY/AxisZ)\tFoliation\tWhatisFolded" : "")
                    + (observationView.selectedEntityType == 2 ? "(East/North/Alt)\tDipDir\tDip" : "")
                    + (observationView.selectedEntityType == 3 ? "(East/North/Alt)\tDipDir\tDip" : "")
                    + (observationView.selectedEntityType == 4 ? "(East/North/Alt)\tDipDir\tDip\tDipPol\tLayer" : "")
                height: mainWindow.defaultFontSize
                font.bold: true
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                verticalAlignment: Text.AlignTop
            }

            ScrollView {
                id: observationView
                property int selectedEntityId: -1
                property int selectedEntityType: -1
                property bool isEntityActive: true
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: visualisationDMHeadings.bottom
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
                    width: visualisationDM.width - mainWindow.myBorders
                    height: eventID == observationView.selectedEntityId ? 30 : 0
                    Text {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.margins: mainWindow.myBorders
                        height: parent.height
                        verticalAlignment: Text.AlignVCenter
                        color: eventID == observationView.selectedEntityId ? "#000000" : "#00000000"
                        font.italic: observationView.isEntityActive ? false : true
                        text: "(" + Math.round(easting) + "," + Math.round(northing) + "," + Math.round(altitude) + ")\t"
                              + (type == 0 ? Math.round(dipDir) + "\t" + dip + "\t" + dipPolarity + "\t" + displacement + "\t" + val : "")
                              + (type == 1 ? "(" + axisX + "," + axisY + "," + axisZ + ")" + "\t" + foliation + "\t" + whatIsFolded : "")
                              + (type == 2 ? dipDir + "\t" + dip : "")
                              + (type == 3 ? dipDir + "\t" + dip  : "")
                              + (type == 4 ? dipDir + "\t" + dip + "\t" + dipPolarity + "\t" + layerName : "")
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
                source: (project.filename === "" ? "" : project.filename + ".png")
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
//            Text {
//                anchors.fill: parent
//                verticalAlignment: Text.AlignTop
//                horizontalAlignment: Text.AlignHCenter
//                text: "(TO BE REPLACED) Map of Region of Interest\nAdd all observations as an overlay so they are selectable and toggled on/off\nHighlight selected event in event log and on map" +
//                "\nNeed a place to show structural information of event, fault area\n of displacement, etc (see Map2Loop notebooks for examples of these diagrams)"
//            }
        }
        Button {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 10
            text: "Reload"
            onPressed: reloadMap()
        }
    }
}
