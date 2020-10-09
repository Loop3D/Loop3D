import QtQuick 2.14
import QtLocation 5.14
import QtPositioning 5.14
import QtQuick.Controls 2.14
import QtQuick.Shapes 1.14
import QtQuick.Dialogs 1.2

Item {
    id: element
    anchors.fill: parent

    function updateMinLatitude(val) { minLatitudeEdit.update(val) }
    function updateMaxLatitude(val) { maxLatitudeEdit.update(val) }
    function updateMinLongitude(val) { minLongitudeEdit.update(val) }
    function updateMaxLongitude(val) { maxLongitudeEdit.update(val) }
    function updateMinNorthing(val) { minNorthingEdit.update(val) }
    function updateMaxNorthing(val) { maxNorthingEdit.update(val) }
    function updateMinEasting(val) { minEastingEdit.update(val) }
    function updateMaxEasting(val) { maxEastingEdit.update(val) }
    function updateMinDepth(val) { minDepthEdit.update(val) }
    function updateMaxDepth(val) { maxDepthEdit.update(val) }
    function updateZone() { utmZoneEdit.text = project.utmZone }

    Text {
        id: roiLabel
        color: "black"
        text: "Extents and Projections"
        font.pixelSize: mainWindow.headingFontSize
        font.family: mainWindow.headingFontStyle
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 2
    }
    Button {
        id: padlockROI
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 4
        width: 30
        height: 20
        onClicked: {
            if (project.hasFilename()) {
                if (project.lockedExtents) {
                    unlockDialog.open()
                }
                else project.setLockedExtents(true)
            } else {
                fileDialogSave.open()
                if (project.hasFilename()) project.setLockedExtents(true)
            }
        }
    }
    Image {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 4
        id: padlockLocked
        visible: project.lockedExtents
        source: "images/locked.png"
        width: padlockROI.width
        height: padlockROI.height
    }
    Image {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 4
        id: padlockUnlocked
        visible: !project.lockedExtents
        source: "images/unlocked.png"
        width: padlockROI.width
        height: padlockROI.height
    }

    // Create boundary cube to set dimensions (Won't be to scale!)
    Shape {
        id: boundaryBox
        anchors.top: roiLabel.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 30
        anchors.rightMargin: 20
        height: parent.height * 2/3
        ShapePath {
            id: path
            property var ratio: 0.618 // or 0.75  // or 0.8 // or 0.61803398875 = (1/GR)
            property var margin: 20
            property var x0: margin
            property var x1: margin + (boundaryBox.width - 2*margin)*(1-ratio)
            property var x2: margin + (boundaryBox.width - 2*margin)*(ratio)
            property var x3: boundaryBox.width - margin
            property var y0: margin
            property var y1: margin + (boundaryBox.height - 2*margin)*(1-ratio)
            property var y2: margin + (boundaryBox.height - 2*margin)*(ratio)
            property var y3: boundaryBox.height - margin
            strokeWidth: 1
            strokeColor: "#000000"
            fillColor: "#55aa0000"
            startX: x0; startY: y1
            PathLine { x: path.x0 ; y: path.y3 }
            PathLine { x: path.x2 ; y: path.y3 }
            PathLine { x: path.x2 ; y: path.y1 }
            PathLine { x: path.x0 ; y: path.y1 }
            PathLine { x: path.x1 ; y: path.y0 }
            PathLine { x: path.x3 ; y: path.y0 }
            PathLine { x: path.x2 ; y: path.y1 }
            PathLine { x: path.x0 ; y: path.y1 }
            PathLine { x: path.x2 ; y: path.y1 }
            PathLine { x: path.x2 ; y: path.y3 }
            PathLine { x: path.x3 ; y: path.y2 }
            PathLine { x: path.x3 ; y: path.y0 }
            PathLine { x: path.x2 ; y: path.y1 }
        }
        Shape {
            ShapePath {
                strokeWidth: 2
                strokeColor: "black"
                fillColor: "#00000000"
                startX: path.x0; startY: (path.y1 + path.y3)/2
                PathLine { x: path.x2; y: (path.y1 + path.y3)/2 }
            }
        }
        Shape {
            ShapePath {
                strokeWidth: 2
                strokeColor: "black"
                fillColor: "#00000000"
                startX: (path.x0+path.x2)/2; startY: path.y1
                PathLine { x: (path.x1+path.x3)/2; y: path.y0 }
            }
        }
        Shape {
            ShapePath {
                strokeWidth: 2
                strokeColor: "black"
                fillColor: "#00000000"
                startX: (path.x2 + path.x3)/2 ; startY: (path.y0 + path.y1)/2
                PathLine { x: (path.x2 + path.x3)/2 ; y: (path.y2 + path.y3)/2 }
            }
        }
        // Add compass Rose to help with orientation
        Shape {
            id: compassRose
            property var offset: (path.x0 + path.x1) / 5
            property var compassWidth: 1.0/20.0
            anchors.fill: parent
            ShapePath {
                strokeWidth: 1
                strokeColor: "black"
                fillColor: "#000000"
                startX: (path.x0 + path.x1) * (2/3+compassRose.compassWidth) - compassRose.offset ; startY: (path.y0 + path.y1) * 1/3
                PathLine { x: (path.x0 + path.x1) * 1/4 - compassRose.offset ; y: (path.y0 + path.y1) * 3/4 }
                PathLine { x: (path.x0 + path.x1) * (2/3-compassRose.compassWidth) - compassRose.offset ; y: (path.y0 + path.y1) * 1/3 }
                PathLine { x: (path.x0 + path.x1) * 3/4 - compassRose.offset ; y: (path.y0 + path.y1) * 1/4 }
                PathLine { x: (path.x0 + path.x1) * (2/3+compassRose.compassWidth) - compassRose.offset ; y: (path.y0 + path.y1) * 1/3 }
            }
            Text {
                x: (path.x0 + path.x1) * 3/4 - compassRose.offset + 5
                y: (path.y0 + path.y1) * 1/4 - 15
                text: "N"
                color: "#000000"
            }
        }

        // Label Axes
        Text {
            color: "black"
            text: inUTM ? "Northing" : "Latitude"
            font.bold: true
            font.family: mainWindow.defaultFontStyle
            font.pixelSize: mainWindow.defaultFontSize
            x: (path.x0 + path.x1)/2
            y: (path.y0 + path.y1)/2
            transform: Rotation { angle: -180 / Math.PI * Math.atan((path.y1-path.y0)/(path.x1-path.x0)) }
        }
        Text {
            color: "black"
            text: inUTM ? "Easting" : "Longitude"
            font.bold: true
            font.family: mainWindow.defaultFontStyle
            font.pixelSize: mainWindow.defaultFontSize
            horizontalAlignment: Text.AlignHCenter
            x: (path.x0 + path.x2)*2/5
            y: path.y2 + (path.y3 - path.y2)
        }
        Text {
            color: "black"
            text: "Depth"
            font.bold: true
            font.family: mainWindow.defaultFontStyle
            font.pixelSize: mainWindow.defaultFontSize
            horizontalAlignment: Text.AlignHCenter
            x: path.x2 + (path.x3 - path.x2)
            y: (path.y0 + path.y2)*2/5
            transform: Rotation { angle: 90 }
        }

        // Region of Interest Geodesic extents
        RoiExtents {
            id: minLatitudeEdit
            visible: !inUTM
            x: (path.x0 + path.x2)/2 - 45
            y: path.y1 - 10
            width: 90
            height: 40
            property real text: project.minLatitude
            property string name: "Min"
            onTextChanged: {
                project.minLatitude = text
                if (!inUTM) project.convertGeodeticToUTM()
            }
        }
        RoiExtents {
            id: maxLatitudeEdit
            visible: !inUTM
            x: (path.x1 + path.x3)/2 - 45
            y: path.y0 - 10
            width: 90
            height: 40
            anchors.margins: 4
            property real text: project.maxLatitude
            property string name: "Max"
            onTextChanged: {
                project.maxLatitude = text
                if (!inUTM) project.convertGeodeticToUTM()
            }
        }
        RoiExtents {
            id: minLongitudeEdit
            visible: !inUTM
            x: path.x0 - 45
            y: (path.y1 + path.y3)/2 - 10
            width: 90
            height: 40
            property real text: project.minLongitude
            property string name: "Min"
            onTextChanged: {
                project.minLongitude = text
                if (!inUTM) project.convertGeodeticToUTM()
            }
        }
        RoiExtents {
            id: maxLongitudeEdit
            visible: !inUTM
            x: path.x2 - 45
            y: (path.y1 + path.y3)/2 - 10
            width: 90
            height: 40
            property real text: project.maxLongitude
            property string name: "Max"
            onTextChanged: {
                project.maxLongitude = text
                if (!inUTM) project.convertGeodeticToUTM()
            }
        }

        // Region of Interest Depth extents
        RoiExtents {
            id: minDepthEdit
            x: (path.x2 + path.x3)/2 - 45
            y: (path.y0 + path.y1)/2 - 10
            height: 40
            width: 90
            property real text: project.minDepth
            property string name: "Min"
            onTextChanged: {
                project.minDepth = text
            }
        }
        RoiExtents {
            id: maxDepthEdit
            x: (path.x2 + path.x3)/2 - 45
            y: (path.y2 + path.y3)/2 - 10
            height: 40
            width: 90
            property real text: project.maxDepth
            property string name: "Max"
            onTextChanged: {
                project.maxDepth = text
            }
        }

        // Region of Interest UTM extents
        RoiExtents {
            id: minNorthingEdit
            visible: inUTM
            x: (path.x0 + path.x2)/2 - 45
            y: path.y1 - 10
            height: 40
            width: 90
            property real text: project.minNorthing
            property string name: "Min"
            onTextChanged: {
                project.minNorthing = text
                if (inUTM) project.convertUTMToGeodetic()
            }
        }
        RoiExtents {
            id: maxNorthingEdit
            visible: inUTM
            x: (path.x1 + path.x3)/2 - 45
            y: path.y0 - 10
            height: 60
            width: 90
            property real text: project.maxNorthing
            property string name: "Max"
            onTextChanged: {
                project.maxNorthing = text
                if (inUTM) project.convertUTMToGeodetic()
            }
        }
        RoiExtents {
            id: minEastingEdit
            visible: inUTM
            x: path.x0 - 45
            y: (path.y1 + path.y3)/2 - 10
            height: 35
            width: 90
            property real text: project.minEasting
            property string name: "Min"
            onTextChanged: {
                project.minEasting = text
                if (inUTM) project.convertUTMToGeodetic()
            }
        }
        RoiExtents {
            id: maxEastingEdit
            visible: inUTM
            x: path.x2 - 45
            y: (path.y1 + path.y3)/2 - 10
            height: 35
            width: 90
            property real text: project.maxEasting
            property string name: "Max"
            onTextChanged: {
                project.maxEasting = text
                if (inUTM) project.convertUTMToGeodetic()
            }
        }

        // Sampling spacing for X,Y,Z
        Rectangle {
            id: spacingX
            x: (path.x0 + path.x2)/2 - 45
            y: (path.y1 + path.y3)/2 - 10
            height: 40
            width: 90
            color: "#00000000"
            anchors.margins: 2
            Text {
                id: sampleSpacingLabelX
                text: "Delta"
                anchors.top: sampleSpacingX.bottom
                anchors.left: parent.left
                horizontalAlignment: Text.AlignHCenter
                height: 20
                width: 90
                font.family: mainWindow.defaultFontStyle
                font.pixelSize: mainWindow.defaultFontSize
                color: "#000000"
                styleColor: "#aaaaaa"
            }
            Rectangle {
                id: sampleSpacingX
                anchors.left: parent.left
                anchors.top: parent.top
                height: 20
                width: 90
                color: "white"
                border.width: 1
                TextInput {
                    readOnly: project.lockedExtents
                    anchors.fill: parent
                    color: "#000000"
                    font.family: mainWindow.defaultFontStyle
                    font.pixelSize: mainWindow.defaultFontSize
                    validator: RegExpValidator { regExp : /[1-9][0-9]+/ }
                    maximumLength: 10
                    horizontalAlignment: Text.AlignHCenter
                    text: project.spacingX
                    onTextChanged: if (text != "" && text != "-" && !project.lockedExtents) project.spacingX = text
                    selectByMouse: true
                }
            }
        }
        Rectangle {
            id: spacingY
            x: (path.x0 + path.x1 + path.x2 + path.x3)/4 - 45
            y: (path.y0 + path.y1)/2 - 10
            height: 40
            width: 90
            color: "#00000000"
            anchors.margins: 2
            Text {
                id: sampleSpacingLabelY
                text: "Delta"
                anchors.top: sampleSpacingY.bottom
                anchors.left: parent.left
                horizontalAlignment: Text.AlignHCenter
                height: 20
                width: 90
                font.family: mainWindow.defaultFontStyle
                font.pixelSize: mainWindow.defaultFontSize
                color: "#000000"
                styleColor: "#aaaaaa"
            }
            Rectangle {
                id: sampleSpacingY
                anchors.left: parent.left
                anchors.top: parent.top
                height: 20
                width: 90
                color: "white"
                border.width: 1
                TextInput {
                    readOnly: project.lockedExtents
                    anchors.fill: parent
                    color: "#000000"
                    font.family: mainWindow.defaultFontStyle
                    font.pixelSize: mainWindow.defaultFontSize
                    validator: RegExpValidator { regExp : /[1-9][0-9]+/ }
                    maximumLength: 10
                    horizontalAlignment: Text.AlignHCenter
                    text: project.spacingY
                    onTextChanged: if (text != "" && text != "-" && !project.lockedExtents) project.spacingY = text
                    selectByMouse: true
                }
            }
        }
        Rectangle {
            id: spacingZ
            x: (path.x2 + path.x3)/2 - 45
            y: (path.y0 + path.y1 + path.y2 + path.y3)/4 - 10
            height: 40
            width: 90
            color: "#00000000"
            anchors.margins: 2
            Text {
                id: sampleSpacingLabelZ
                text: "Delta"
                anchors.top: sampleSpacingZ.bottom
                anchors.left: parent.left
                horizontalAlignment: Text.AlignHCenter
                height: 20
                width: 90
                font.family: mainWindow.defaultFontStyle
                font.pixelSize: mainWindow.defaultFontSize
                color: "#000000"
                styleColor: "#aaaaaa"
            }
            Rectangle {
                id: sampleSpacingZ
                anchors.left: parent.left
                anchors.top: parent.top
                height: 20
                width: 90
                color: "white"
                border.width: 1
                TextInput {
                    readOnly: project.lockedExtents
                    anchors.fill: parent
                    color: "#000000"
                    font.family: mainWindow.defaultFontStyle
                    font.pixelSize: mainWindow.defaultFontSize
                    validator: RegExpValidator { regExp : /[1-9][0-9]+/ }
                    maximumLength: 10
                    horizontalAlignment: Text.AlignHCenter
                    text: project.spacingZ
                    onTextChanged: if (text != "" && text != "-" && !project.lockedExtents) project.spacingZ = text
                    selectByMouse: true
                }
            }
        }
    }

    // Projection Toggle and UTM settings
    Rectangle {
        id: utmSelect
        anchors.left: parent.left
        anchors.top: boundaryBox.bottom
        anchors.margins: 4
        height: 40
        Text {
            id: projectionTag
            anchors.left: parent.left
            text: "in "
            anchors.leftMargin: 5
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.family: mainWindow.defaultFontStyle
            font.pixelSize: mainWindow.defaultFontSize
            width: 20
        }
        Button {
            id: projection
            anchors.left: projectionTag.right
            height: parent.height
            font.family: mainWindow.defaultFontStyle
            font.pixelSize: mainWindow.defaultFontSize
            text: inUTM ? "UTM" : "GEODETIC"
            onClicked: {
                // Ensure reprojection before swap
                if(!project.lockedExtents && project.utmZone != 0) {
                    roiReproject()
                    reCentreMap()
                }
                inUTM = !inUTM
                project.inUtm = inUTM
            }
        }
        Rectangle {
            id: highlightExtent
            visible: inUTM
            anchors.left: projection.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 5
            height: 20
            width: 60
            color: "white"
            border.width: 1
            TextInput {
                id: utmZoneEdit
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                text: project.utmZone
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.left: parent.left
                font.family: mainWindow.defaultFontStyle
                font.pixelSize: mainWindow.defaultFontSize
                validator: RegExpValidator { regExp : /[1-9]|[1-5][0-9]|60/ }
                onTextChanged: if (text != "") project.utmZone = text
            }
        }
        Button {
            id: northSouthSelect
            visible: inUTM
            anchors.left: highlightExtent.right
            height: parent.height
            width: 20
            font.family: mainWindow.defaultFontStyle
            font.pixelSize: mainWindow.defaultFontSize
            text: project.utmNorthSouthStr
            onClicked: {
                if (project.utmNorthSouth !== 1) {
                    project.utmNorthSouthStr = "N"
                    project.utmNorthSouth = 1
                } else {
                    project.utmNorthSouthStr = "S"
                    project.utmNorthSouth = 0
                }
            }
        }
    }

    // Reset extents from geodetic to utm and vice versa
    Button {
        id: resetRoi
        anchors.right: parent.right
        anchors.top: boundaryBox.bottom
        anchors.margins: 4
        height: 40
        text: "Clear Extents"
        onClicked: {
            if(!project.lockedExtents) {
                project.clearProject()
                roiReproject()
            }
        }
    }

    // Notification of dimensions of chosen extents
    Text {
        id: size
        anchors.top: utmSelect.bottom
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        width: 200
        font.family: mainWindow.defaultFontStyle
        font.pixelSize: mainWindow.defaultFontSize
        color: "#000000"
        text: "Approx. Dimensions (m) = " + (project.maxEasting - project.minEasting).toFixed(0) +
              " x " + (project.maxNorthing - project.minNorthing).toFixed(0) +
              " x " + (project.maxDepth - project.minDepth).toFixed(0) +
              "\n Grid Size = "  + ((project.maxEasting - project.minEasting)/project.spacingX).toFixed(0) +
              " x " + ((project.maxNorthing - project.minNorthing)/project.spacingY).toFixed(0) +
              " x " + ((project.maxDepth - project.minDepth)/project.spacingZ).toFixed(0)
    }

    // Dialog box for unlocking extents
    MessageDialog {
        id: unlockDialog
        title: "Are you sure?"
        icon: StandardIcon.Question
        informativeText: "You might have data and/or models already created with these extents.\n"
            + "Changing them will require these to be re-calculated.\n\n"
            + "Continuing will delete the current project file and re-create an empty one.\n"
            + "Do you want to continue?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            project.deleteProject()
            project.clearProject(false)
            project.saveProject()
            project.setLockedExtents(false)
        }
    }

    Component.onCompleted: reCentreMap()
}
