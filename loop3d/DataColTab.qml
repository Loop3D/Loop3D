import QtQuick 2.14
import QtLocation 5.14
import QtPositioning 5.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import loop3d.pythontext 1.0
import loop3d.utmconverter 1.0
import loop3d.datasourcemodel 1.0
import loop3d.m2lconfig 1.0

Item {
    id: dcTab

    // Core region of interest values
    property bool inUTM: false
    property real mapCentreLatitude: 0
    property real mapCentreLongitude: 0
    Component.onCompleted: {
        reCentreMap()
    }

    function refreshSourceModel() {
        dataSourceModel.refreshModel()
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
        roi.updateTopExtent(project.topExtent)
        roi.updateBotExtent(project.botExtent)
        roi.updateZone()
    }

    function runExtractData() {
        project.saveProject()
        project.pythonErrors = ""
        project.pythonInProgress = 0.01
        dcPythonText.run(dcTextArea.text,project.filename,"DataCollection")
    }

    function finishedExtractData() {
        if (project.pythonErrors) {
            console.log(project.pythonErrors)
//            pythonErrorDialog.open()
        } else {
            bar.currentIndex = 1
            dmTab.reloadMap()
        }
    }

    // re-centre the map function from geodetic extents
    function reCentreMap() {
        mapCentreLatitude = (project.minLatitude + project.maxLatitude) / 2
        // Ensure change in centre location to force recentre
        mapCentreLongitude = (project.minLongitude + project.maxLongitude) / 2 - 1
        mapCentreLongitude = (project.minLongitude + project.maxLongitude) / 2
        mapUnderlayer.fitViewportToMapItems()
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

        // Map information using open street view
        // Needs online access to function
        Map {
            id: mapUnderlayer
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            plugin: Plugin { name: "osm" }
            center: QtPositioning.coordinate(mapCentreLatitude, mapCentreLongitude)
            zoomLevel: 6
            maximumZoomLevel: 10
            MouseArea {
                id: mapInteraction
                anchors.fill: parent
                propagateComposedEvents: false
                hoverEnabled: true
                pressAndHoldInterval: 0
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                property int lastX: -1
                property int lastY: -1
                property bool drawRegion: false
                UTM { id: llConvert }
                LL { id: llPos }
                Location { id: llLoc }
                Location { id: startPos }
                Location { id: endPos }

                onPressed: {
                    lastX = mouse.x
                    lastY = mouse.y
                    if (mouse.button === Qt.RightButton && !project.lockedExtents) {
                        project.utmZone = 0
                        startPos.coordinate = mapUnderlayer.toCoordinate(Qt.point(mouseX, mouseY))
                        endPos.coordinate = mapUnderlayer.toCoordinate(Qt.point(mouseX, mouseY))
                        mapUnderlayer.gesture.acceptedGestures = 0
                        drawRegion = !drawRegion
                    }
                }
                onReleased: {
                    if (mouse.button === Qt.RightButton && !project.lockedExtents) {
                        endPos.coordinate = mapUnderlayer.toCoordinate(Qt.point(mouseX, mouseY))
                        roiReproject()
                        drawRegion = !drawRegion
                    }
                    mapUnderlayer.gesture.acceptedGestures = 31
                }

                onPositionChanged: {
                    if (drawRegion) {
                        endPos.coordinate = mapUnderlayer.toCoordinate(Qt.point(mouseX, mouseY))

                        // Check for NaNs (i.e. when mouse is dragged outside map area
                        if (endPos.coordinate.latitude === endPos.coordinate.latitude && endPos.coordinate.longitude === endPos.coordinate.longitude) {
                            // Proceed as no NaNs detected
                            project.updateGeodeticLimits(startPos.coordinate.latitude, endPos.coordinate.latitude, startPos.coordinate.longitude, endPos.coordinate.longitude)
                            roiReproject()
                        }
                    }

                    llLoc.coordinate = mapUnderlayer.toCoordinate(Qt.point(mouseX, mouseY))
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
            MapOverlayLayer {}
        }
        Map {
            id: mapBaseWA
            enabled: project.activeState === 1
            visible: project.activeState === 1
            gesture.acceptedGestures: MapGestureArea.NoGesture
            layer.enabled: true
            opacity: geologyLayerTransparency.value
            anchors.fill: mapUnderlayer
            color: "#00000000"
            plugin: Plugin { name: "osm"
                PluginParameter { name: "osm.mapping.offline.directory"; value: workingDir + "/offline_tiles/wa/" }
            }
            center: mapUnderlayer.center
            zoomLevel: mapUnderlayer.zoomLevel
            maximumZoomLevel: mapUnderlayer.maximumZoomLevel
            MapOverlayLayer {}
        }
        Map {
            id: mapBaseSA
            enabled: project.activeState === 2
            visible: project.activeState === 2
            gesture.acceptedGestures: MapGestureArea.NoGesture
            anchors.fill: mapUnderlayer
            layer.enabled: true
            opacity: geologyLayerTransparency.value
            color: "#00000000"
            plugin: Plugin { name: "osm"
                PluginParameter { name: "osm.mapping.offline.directory"; value: workingDir + "/offline_tiles/sa/" }
            }
            center: mapUnderlayer.center
            zoomLevel: mapUnderlayer.zoomLevel
            maximumZoomLevel: mapUnderlayer.maximumZoomLevel
            MapOverlayLayer {}
        }
        Map {
            id: mapBaseNT
            enabled: project.activeState === 3
            visible: project.activeState === 3
            gesture.acceptedGestures: MapGestureArea.NoGesture
            anchors.fill: mapUnderlayer
            layer.enabled: true
            opacity: geologyLayerTransparency.value
            color: "#00000000"
            plugin: Plugin { name: "osm"
                PluginParameter { name: "osm.mapping.offline.directory"; value: workingDir + "/offline_tiles/nt/" }
            }
            center: mapUnderlayer.center
            zoomLevel: mapUnderlayer.zoomLevel
            maximumZoomLevel: mapUnderlayer.maximumZoomLevel
            MapOverlayLayer {}
        }
        Map {
            id: mapBaseQLD
            enabled: project.activeState === 4
            visible: project.activeState === 4
            gesture.acceptedGestures: MapGestureArea.NoGesture
            anchors.fill: mapUnderlayer
            layer.enabled: true
            opacity: geologyLayerTransparency.value
            color: "#00000000"
            plugin: Plugin { name: "osm"
                PluginParameter { name: "osm.mapping.offline.directory"; value: workingDir + "/offline_tiles/qld/" }
            }
            center: mapUnderlayer.center
            zoomLevel: mapUnderlayer.zoomLevel
            maximumZoomLevel: mapUnderlayer.maximumZoomLevel
            MapOverlayLayer {}
        }
        Map {
            id: mapBaseNSW
            enabled: project.activeState === 5
            visible: project.activeState === 5
            gesture.acceptedGestures: MapGestureArea.NoGesture
            anchors.fill: mapUnderlayer
            layer.enabled: true
            opacity: geologyLayerTransparency.value
            color: "#00000000"
            plugin: Plugin { name: "osm"
                PluginParameter { name: "osm.mapping.offline.directory"; value: workingDir + "/offline_tiles/nsw/" }
            }
            center: mapUnderlayer.center
            zoomLevel: mapUnderlayer.zoomLevel
            maximumZoomLevel: mapUnderlayer.maximumZoomLevel
            MapOverlayLayer {}
        }
        Map {
            id: mapBaseVIC
            enabled: project.activeState === 6
            visible: project.activeState === 6
            gesture.acceptedGestures: MapGestureArea.NoGesture
            anchors.fill: mapUnderlayer
            layer.enabled: true
            opacity: geologyLayerTransparency.value
            color: "#00000000"
            plugin: Plugin { name: "osm"
                PluginParameter { name: "osm.mapping.offline.directory"; value: workingDir + "/offline_tiles/vic/" }
            }
            center: mapUnderlayer.center
            zoomLevel: mapUnderlayer.zoomLevel
            maximumZoomLevel: mapUnderlayer.maximumZoomLevel
            MapOverlayLayer {}
        }
        Map {
            id: mapBaseTAS
            enabled: project.activeState === 7
            visible: project.activeState === 7
            gesture.acceptedGestures: MapGestureArea.NoGesture
            anchors.fill: mapUnderlayer
            layer.enabled: true
            opacity: geologyLayerTransparency.value
            color: "#00000000"
            plugin: Plugin { name: "osm"
                PluginParameter { name: "osm.mapping.offline.directory"; value: workingDir + "/offline_tiles/tas/" }
            }
            center: mapUnderlayer.center
            zoomLevel: mapUnderlayer.zoomLevel
            maximumZoomLevel: mapUnderlayer.maximumZoomLevel
            MapOverlayLayer {}
        }
        Rectangle {
            id: mapControls
            layer.enabled: true
            color: "#00000000"
            anchors.fill: mapUnderlayer
            Slider {
                id: geologyLayerTransparency
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 4
                width: 100
                height: 20
                from: 0.0
                to: 1.0
                value: 0.5
            }
            Button {
                property string activeState: "WA"
                id: mapLayer
                anchors.top: parent.top
                anchors.right: parent.right
                width: 100
                height: 30
                text: project.activeStateName
                onPressed: {
                    project.activeState = (project.activeState + 1) % 8
                    if (project.activeState === 0) project.activeStateName = "--"
                    else if (project.activeState === 1) project.activeStateName = "WA"
                    else if (project.activeState === 2) project.activeStateName = "SA"
                    else if (project.activeState === 3) project.activeStateName = "NT"
                    else if (project.activeState === 4) project.activeStateName = "QLD"
                    else if (project.activeState === 5) project.activeStateName = "NSW"
                    else if (project.activeState === 6) project.activeStateName = "VIC"
                    else if (project.activeState === 7) project.activeStateName = "TAS"
                    dataSourceModel.selectItemById(project.activeStateName,true)
                }
            }
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
                id: detailsStack
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - 60
                width: parent.width
                currentIndex: dcDetailsBar.currentIndex

                Rectangle {
                    id: dcSourceTab
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
                        property string dsfilename: "DataSource.conf"
                        dataSources: dataSourceList
                        Component.onCompleted: {
                            loadDataSources(dsfilename)
                            dataSourceModel.selectItemById("WA",true)
                        }
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
                            color: isParent ? "lightblue" : selected == true ? "grey" : "lightgrey"
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
                                verticalAlignment: Text.AlignVCenter
                                height: parent.height
                                text: name + "\t" + url
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
                            CheckBox {
                                visible: !isParent && height != 0
                                height: parent.height
                                anchors.right: parent.right
                                anchors.top: parent.top
                                tristate: false
                                checkable: !project.lockedExtents
                                checkState: selected ? Qt.Checked : Qt.Unchecked
                                onToggled: dataSourceModel.selectItem(index,!selected)
                            }
                        }
                    }
                }

                Rectangle {
                    id: dcOptionsTab
                    color: "#bbbbbb"

                    Flickable {
                        anchors.fill: parent
                        contentHeight: dcOptionsGrid.height
                        clip: true
                        GridLayout {
                            id: dcOptionsGrid
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 5
                            columns: 2
                            rowSpacing: 2

                            Label { text: "Quiet Mode" }
                            ComboBox {
                                Layout.minimumWidth: 200
                                currentIndex: m2lConfig.quietMode
                                model: ["Quiet","No Figures", "Show All"]
                                flat: true
                                onCurrentIndexChanged: m2lConfig.quietMode = currentIndex
                            }
                            Label { text: "Orientation Decimate" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.orientationDecimate
                                onTextChanged: m2lConfig.orientationDecimate = text
                                validator: IntValidator { bottom: 0; top: 9999 }
                            }
                            Label { text: "Contact Decimate" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.contactDecimate
                                onTextChanged: m2lConfig.contactDecimate = text
                                validator: IntValidator { bottom: 0; top: 9999 }
                            }
                            Label { text: "Intrusion Mode" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.intrusionMode
                                onTextChanged: m2lConfig.intrusionMode = text
                                validator: IntValidator { bottom: 0; top: 1 }
                            }
                            Label { text: "Interpolation Spacing" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.interpolationSpacing
                                onTextChanged: m2lConfig.interpolationSpacing = text
                                validator: IntValidator { bottom: 0; top: 99999 }
                            }
                            Label { text: "Misorientation" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.misorientation
                                onTextChanged: m2lConfig.misorientation = text
                                validator: IntValidator { bottom: 0; top: 5000 }
                            }
                            Label { text: "Interpolation Scheme" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.interpolationScheme
                                onTextChanged: m2lConfig.interpolationScheme = text
                            }
                            Label { text: "Fault Decimate" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.faultDecimate
                                onTextChanged: m2lConfig.faultDecimate = text
                                validator: IntValidator { bottom: 0; top: 9999 }
                            }
                            Label { text: "Minimum Fault Length" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.minFaultLength
                                onTextChanged: m2lConfig.minFaultLength = text
                                validator: IntValidator { bottom: 0; top: 99999999 }
                            }
                            Label { text: "Fault Dip" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.faultDip
                                onTextChanged: m2lConfig.faultDip = text
                                validator: IntValidator { bottom: -90; top: 90 }
                            }
                            Label { text: "Pluton Dip" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.plutonDip
                                onTextChanged: m2lConfig.plutonDip = text
                                validator: IntValidator { bottom: -90; top: 90 }
                            }
                            Label { text: "Pluton Form" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.plutonForm
                                onTextChanged: m2lConfig.plutonForm = text
                            }
                            Label { text: "Dist Buffer" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.distBuffer
                                onTextChanged: m2lConfig.distBuffer = text
                                validator: IntValidator { bottom: 0; top: 99999 }
                            }
                            Label { text: "Contact Dip" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.contactDip
                                onTextChanged: m2lConfig.contactDip = text
                                validator: IntValidator { bottom: -90; top: 90 }
                            }
                            Label { text: "Contact Orientation Decimate" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.contactOrientationDecimate
                                onTextChanged: m2lConfig.contactOrientationDecimate = text
                                validator: IntValidator { bottom: 0; top: 9999 }
                            }
                            Label { text: "Null Scheme" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.nullScheme
                                onTextChanged: m2lConfig.nullScheme = text
                            }
                            Label { text: "Thickness Buffer" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.thicknessBuffer
                                onTextChanged: m2lConfig.thicknessBuffer = text
                                validator: IntValidator { bottom: 0; top: 99999 }
                            }
                            Label { text: "Maximum Thickness Allowed" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.maxThicknessAllowed
                                onTextChanged: m2lConfig.maxThicknessAllowed = text
                                validator: IntValidator { bottom: 0; top: 99999 }
                            }
                            Label { text: "Fold Decimate" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.foldDecimate
                                onTextChanged: m2lConfig.foldDecimate = text
                                validator: IntValidator { bottom: 0; top: 9999 }
                            }
                            Label { text: "Fat Step" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.fatStep
                                onTextChanged: m2lConfig.fatStep = text
                                validator: IntValidator { bottom: 0; top: 99999 }
                            }
                            Label { text: "Close Dip" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.closeDip
                                onTextChanged: m2lConfig.closeDip = text
                                validator: IntValidator { bottom: -999; top: 999 }
                            }
                            Label { text: "Use Interpolations" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.useInterpolations
                                onTextChanged: m2lConfig.useInterpolations = text
                            }
                            Label { text: "Use Fat" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: m2lConfig.useFat
                                onTextChanged: m2lConfig.useFat = text
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
                                filename: "processMap2Loop.py"
                                Component.onCompleted: {
                                    textDocToHighlight = dcTextArea.textDocument
                                    // Use select all and text addition to trigger immediate highlighting
                                    dcTextArea.selectAll()
                                    dcTextArea.text += " "
                                }
                            }
                        }
                    }
                }
            }
            TabBar {
                id: dcDetailsBar
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                padding: 4
                TabButton { text: 'Data Sources' }
                TabButton { text: 'Options' }
                TabButton { text: 'Python Code' }
            }
            Button {
                id: dcRunCode
                anchors.bottom: detailsStack.bottom
                anchors.right: detailsStack.right
                height: 80
                width: 140
                text: dcDetailsBar.currentIndex == 2 ? "Run Code" :"Extract Data"
                onPressed: {
                    // Ensure there is a Loop Project File specified to save to before starting extraction
                    if (!project.hasFilename()) {
                        fileDialogSave.open()
                        waitingToExtract = true
                    } else runExtractData()
                }
            }
        }
    }
}



/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
