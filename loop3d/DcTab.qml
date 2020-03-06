import QtQuick 2.12
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import loop3d.pythontext 1.0
import loop3d.utmconverter 1.0
import loop3d.datasourcemodel 1.0

//import Qt3D.Core 2.12
//import Qt3D.Render 2.12
//import QtQuick.Scene3D 2.12

Item {
    id: dcTab

    // Core region of interest values
    property bool inUTM: false
    property bool lockRegionOfInterest: false
    property real mapCentreLatitude: 0
    property real mapCentreLongitude: 0
    Component.onCompleted: {
        reCentreMap()
    }

    function roiReproject() {
        roi.updateMinLatitude(project.minLatitude)
        roi.updateMaxLatitude(project.maxLatitude)
        roi.updateMinLongitude(project.minLongitude)
        roi.updateMaxLongitude(project.maxLongitude)
        roi.updateMaxEasting(project.maxEasting)
        roi.updateMinEasting(project.minEasting)
        roi.updateMaxNorthing(project.maxNorthing)
        roi.updateMinNorthing(project.minNorthing)
        roi.updateMinDepth(project.minDepth)
        roi.updateMaxDepth(project.maxDepth)
        roi.updateZone()
    }

    // re-centre the map function from geodetic extents
    function reCentreMap() {
        mapCentreLatitude = (project.minLatitude + project.maxLatitude) / 2
        // Ensure change in centre location to force recentre
        mapCentreLongitude = (project.minLongitude + project.maxLongitude) / 2 - 1
        mapCentreLongitude = (project.minLongitude + project.maxLongitude) / 2
        mapBase.fitViewportToMapItems()
    }

    // Main area for data collection tab
    Rectangle {
        border.color: "#ffffff"
        border.width: 0
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#a1a1f0"
            }
            GradientStop {
                position: 1
                color: "#0f0f82"
            }
        }

        // Configuration area contains region of interest and projection info
        Rectangle {
            id: configDC
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#dddddd"
            clip: true
            RegionOfInterest {
                id: roi
                anchors.fill: parent
            }
        }

        // Initiate map plugin
        Plugin {
            id: mapPlugin
            name: "osm"
        }

        // Map information using open street view
        // Needs online access to function
        Map {
            id: mapBase
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            plugin: mapPlugin
            center: QtPositioning.coordinate(mapCentreLatitude, mapCentreLongitude)
            zoomLevel: 7
            MapPolygon {
                id: regionOfInterestPoly
                color: "#55aa0000"
                path: [
                    QtPositioning.coordinate(project.minLatitude, project.minLongitude),
                    QtPositioning.coordinate(project.maxLatitude, project.minLongitude),
                    QtPositioning.coordinate(project.maxLatitude, project.maxLongitude),
                    QtPositioning.coordinate(project.minLatitude, project.maxLongitude)
                ]
                border.width: 2
            }
        }

        MouseArea {
            id: mapInteraction
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            // hover enables events without clicks
            hoverEnabled: true
            pressAndHoldInterval: 0
            acceptedButtons: Qt.LeftButton | Qt.RightButton


            property int lastX: -1
            property int lastY: -1
            property bool panning: false
            property bool drawRegion: false
            UTM { id: llConvert }
            LL { id: llPos }
            Location { id: llLoc }
            Location { id: startPos }
            Location { id: endPos }

            onPressed: {
                lastX = mouse.x
                lastY = mouse.y
            }
            onPressAndHold: {
                if (mouse.button === Qt.LeftButton) {
                    panning = true
                }
                if (mouse.button === Qt.RightButton && !lockRegionOfInterest) {
                    project.utmZone = 0
                    startPos.coordinate = mapBase.toCoordinate(Qt.point(mouseX, mouseY))
                    endPos.coordinate = mapBase.toCoordinate(Qt.point(mouseX, mouseY))
                    drawRegion = true
                }
            }
            onReleased: {
                if (mouse.button === Qt.RightButton && !lockRegionOfInterest) {
                    endPos.coordinate = mapBase.toCoordinate(Qt.point(mouseX, mouseY))
                    roiReproject()
                }
                panning = false
                drawRegion = false
            }

            onPositionChanged: {
                if (panning) {
                    mapBase.pan(lastX-mouseX, lastY-mouseY)
                    lastX = mouseX
                    lastY = mouseY
                }
                if (drawRegion) {
                    endPos.coordinate = mapBase.toCoordinate(Qt.point(mouseX, mouseY))

                    // Check for NaNs (i.e. when mouse is dragged outside map area
                    if (endPos.coordinate.latitude === endPos.coordinate.latitude && endPos.coordinate.longitude === endPos.coordinate.longitude) {
                        // Proceed as no NaNs detected
                        project.updateGeodeticLimits(startPos.coordinate.latitude, endPos.coordinate.latitude, startPos.coordinate.longitude, endPos.coordinate.longitude)
                        roiReproject()
                    }
                }

                llLoc.coordinate = mapBase.toCoordinate(Qt.point(mouseX, mouseY))
                if (project.inUtm) {
                    llPos.latitude = llLoc.coordinate.latitude
                    llPos.longitude = llLoc.coordinate.longitude
                    llConvert.fromLL(llPos)
                    notifyText.text = "Mouse Position: Zone:"+ llConvert.zone + llConvert.northStr + " N:" + llConvert.northing.toFixed(1) + " S:" + llConvert.easting.toFixed(1)
                } else {
                    notifyText.text = "Mouse Position: " + llLoc.coordinate.toString().match(/.*[SN].*[EW]/)[0]
                }
            }
            onExited: notifyText.text = ""
        }

        // Details window for selecting and viewing data appropriate for this
        // project
        Rectangle {
            id: detailsDC
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            color: "#bbbbbb"

            StackLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - 60
                width: parent.width
                currentIndex: dcDetailsBar.currentIndex

                Rectangle {
                    id: sourceTab
                    color: "#bbbbbb"
                    Text {
                        id: detailsDCHeader
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: mainWindow.myBorders
                        height: 20
                        text: "Data Sources"
                        font.bold: true
                        font.family: mainWindow.headingFontStyle
                        font.pixelSize: mainWindow.headingFontSize
                        horizontalAlignment: Text.AlignHCenter
                    }
                    DataSourceModel {
                        id: dataSourceModel
                        property string dsfilename: "loop3d/DataSource.conf"
                        dataSources: dataSourceList
                        Component.onCompleted: loadDataSources(dsfilename)
                    }

                    ListView {
                        id: listing
                        anchors.top: detailsDCHeader.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: mainWindow.myBorders
                        model: dataSourceModel
                        clip: true

                        delegate: listdelegate
                    }

                    Component {
                        id: listdelegate

                        Rectangle {
                            id: menuItem
                            property int itemHeight: 30
                            width: parent.width
                            height: isParent ? itemHeight : 0
                            color: isParent ? "lightblue" : "white"
                            border.width: 1

                            states: State {
                                name: "expanded"
                                when: isExpanded
                                PropertyChanges {
                                    target: menuItem
                                    height: itemHeight
                                }
                            }

                            transitions:[
                                Transition {
                                    from: ""
                                    to: "expanded"
                                    reversible: true
                                    SequentialAnimation {
                                        PropertyAnimation { property: "height"; duration: 200 }
                                    }
                                }
                            ]

                            Image {
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.margins: 5
                                id: stateCollapsed
                                visible: !isExpanded && isParent
                                source: "images/collapsed.png"
                                width: 20
                                height: 20
                            }
                            Image {
                                anchors.left: parent.left
                                anchors.top: parent.top
                                anchors.margins: 5
                                id: stateExpanded
                                visible: isExpanded && isParent
                                source: "images/expanded.png"
                                width: 20
                                height: 20
                            }
                            Text {
                                id: text
                                verticalAlignment: Text.AlignVCenter
                                height: parent.height
                                text: name
                                clip: true
                                anchors.left: stateExpanded.right
                                anchors.margins: mainWindow.myBorders
                                font.family: mainWindow.defaultFontStyle
                                font.pixelSize: mainWindow.defaultFontSize
                            }

                            MouseArea {
                                anchors.fill: parent
                                propagateComposedEvents: true
                                onClicked: {
                                    if (isParent) dataSourceModel.expandGroup(group)
                                    else mouse.accepted = false
                                }
                            }
                            // Ensure button is after MouseArea so that it is layered above the mouse capture event
                            Button {
                                id: statusButton
                                visible: !isParent
                                height: parent.height
                                text: (dlState == "" ? "Download": dlState)
                                anchors.right: parent.right
                                anchors.margins: 2
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom
                                width: 150
                                onClicked: {
                                    if (dlState == "") {
                                        dlState = "downloading"
                                    } else {
                                        dlState = ""
                                    }
                                }
                            }
                        }
                    }
                    Button {
                        id: dcExtractDataButton
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        height: 80
                        width: 140
                        text: "Extract Data"
                        onPressed: {
                            // TODO: remove file open when finished testing textures
                            if (!hasFile) fileDialogOpen.open()
                            else {
                                dcPythonText.run(dcTextArea.text,project.filename)
                                project.reloadProject()
                                bar.currentIndex = 1
                            }
                        }
                    }
                }
                Rectangle {
                    id: dcCodeTab
                    color: "#bbbbbb"

                    ScrollView {
                        id: sv
                        anchors.fill: parent
                        anchors.margins: 4
                        clip: true
                        TextEdit {
                            anchors.fill: parent
                            id: dcTextArea
                            text: dcPythonText.pythonCode
                            selectByMouse: true
                            PythonText {
                                id: dcPythonText
                                filename: "map2loopTemplate.py"
                                Component.onCompleted: {
                                    textDocToHighlight = dcTextArea.textDocument
                                    // Use select all and text addition to trigger immediate highlighting
                                    dcTextArea.selectAll()
                                    dcTextArea.text += " "
                                }
                            }
                        }
                    }
                    Button {
                        id: dcRunCode
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        height: 80
                        width: 140
                        text: "Run Code"
                        onPressed: {
                            // TODO: remove file open when finished testing textures
                            if (!hasFile) fileDialogOpen.open()
                            else {
                                dcPythonText.run(dcTextArea.text,project.filename)
                                project.reloadProject()
                                bar.currentIndex = 1
//                                dcDetailsBar.currentIndex = 2
                            }
                        }
                    }
                }

                Rectangle {
//                    anchors.fill: parent
                    id: observationsTab
                    color: "#bbbbbb"
                    Text {
                        anchors.fill: parent
                        text: "Once run list of observations extracted from geology model"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    // Needs Qt 5.14 for embedding Scene3D into QQuickViews
//                    Scene3D {
////                        compositingMode: Scene3D.FBO
//                        Entity {
//                            RenderSettings {
//                                activeFrameGraph: RenderSurfaceSelector {
//                                    Viewport {
//                                        id: mainViewport
//                                        normalizedRect: Qt.rect(0.5, 0.5, 1, 1)
//                                        CameraSelector { id: cameraViewport }

//                                        ClearBuffers {
//                                            buffers: ClearBuffers.ColorDepthBuffer
//                                            clearColor: Qt.rgba(0.6, 0.6, 1.0, 0.1)
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }
                }

            }
            TabBar {
                id: dcDetailsBar
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                padding: 4
                TabButton { text: 'Data Sources' }
                TabButton { text: 'Python Code' }
//                TabButton { text: 'Observations' }
            }
        }
    }
}


