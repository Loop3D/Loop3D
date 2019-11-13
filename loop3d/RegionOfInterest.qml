import QtQuick 2.0
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick.Controls 2.12

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
        onClicked: lockRegionOfInterest = !lockRegionOfInterest
    }
    Image {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 4
        id: padlockLocked
        visible: lockRegionOfInterest
        source: "images/locked.png"
        width: padlockROI.width
        height: padlockROI.height
    }
    Image {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 4
        id: padlockUnlocked
        visible: !lockRegionOfInterest
        source: "images/unlocked.png"
        width: padlockROI.width
        height: padlockROI.height
    }

    Rectangle {
        id: roiBackground
        color: "#55aa0000"
        anchors.top: roiLabel.bottom
        anchors.margins: 14
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 3 / 4
        height: 65
        border.width: 1
        border.color: "black"
    }

    // Region of Interest Geodetic extents
    RoiExtents {
        id: maxLatitudeEdit
        visible: !inUTM
        anchors.top: roiLabel.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: 40
        width: 90
        anchors.margins: 4
        property real text: project.maxLatitude
        property string name: "Max Latitude"
        onTextChanged: {
            project.maxLatitude = text
            if (!inUTM) project.convertGeodeticToUTM()
        }
    }
    RoiExtents {
        id: minLatitudeEdit
        visible: !inUTM
        anchors.top: maxLongitudeEdit.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 4
        height: 40
        width: 90
        property real text: project.minLatitude
        property string name: "Min Latitude"
        onTextChanged: {
            project.minLatitude = text
            if (!inUTM) project.convertGeodeticToUTM()
        }
    }
    RoiExtents {
        id: maxLongitudeEdit
        visible: !inUTM
        anchors.top: roiLabel.bottom
        anchors.right: parent.right
        anchors.margins: 30
        height: 35
        width: 90
        property real text: project.maxLongitude
        property string name: "Max Longitude"
        onTextChanged: {
            project.maxLongitude = text
            if (!inUTM) project.convertGeodeticToUTM()
        }
    }
    RoiExtents {
        id: minLongitudeEdit
        visible: !inUTM
        anchors.top: roiLabel.bottom
        anchors.left: parent.left
        anchors.margins: 30
        height: 35
        width: 90
        property real text: project.minLongitude
        property string name: "Min Longitude"
        onTextChanged: {
            project.minLongitude = text
            if (!inUTM) project.convertGeodeticToUTM()
        }
    }

    // Region of Interest UTM extents
    RoiExtents {
        id: maxNorthingEdit
        visible: inUTM
        anchors.top: roiLabel.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: 60
        width: 90
        anchors.margins: 4
        property real text: project.maxNorthing
        property string name: "Max Northing"
        onTextChanged: {
            project.maxNorthing = text
            if (inUTM) project.convertUTMToGeodetic()
        }
    }
    RoiExtents {
        id: minNorthingEdit
        visible: inUTM
        anchors.top: maxEastingEdit.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 4
        height: 40
        width: 90
        property real text: project.minNorthing
        property string name: "Min Northing"
        onTextChanged: {
            project.minNorthing = text
            if (inUTM) project.convertUTMToGeodetic()
        }
    }
    RoiExtents {
        id: maxEastingEdit
        visible: inUTM
        anchors.top: roiLabel.bottom
        anchors.right: parent.right
        anchors.margins: 30
        height: 35
        width: 90
        property real text: project.maxEasting
        property string name: "Max Easting"
        onTextChanged: {
            project.maxEasting = text
            if (inUTM) project.convertUTMToGeodetic()
        }
    }
    RoiExtents {
        id: minEastingEdit
        visible: inUTM
        anchors.top: roiLabel.bottom
        anchors.left: parent.left
        anchors.margins: 30
        height: 35
        width: 90
        property real text: project.minEasting
        property string name: "Min Easting"
        onTextChanged: {
            project.minEasting = text
            if (inUTM) project.convertUTMToGeodetic()
        }
    }

    // Reset extents from geodetic to utm and vice versa
    Button {
        id: resetRoi
        visible: inUTM
        anchors.right: parent.right
        anchors.top: roiBackground.bottom
        anchors.margins: 4
        height: 40
        text: "Clear Projection"
        onClicked: {
            if(!lockRegionOfInterest) {
                project.clearProject()
                roiReproject()
            }
        }
    }

    // Projection Toggle and UTM settings
    Rectangle {
        id: utmSelect
        anchors.left: parent.left
        anchors.top: roiBackground.bottom
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
            font.family: mainWindow.headingFontStyle
            font.pixelSize: mainWindow.headingFontSize
            width: 20
        }
        Button {
            id: projection
            anchors.left: projectionTag.right
            height: parent.height
            font.family: mainWindow.headingFontStyle
            font.pixelSize: mainWindow.headingFontSize
            text: inUTM ? "UTM" : "GEODETIC"
            onClicked: {
                // Ensure reprojection before swap
                if(!lockRegionOfInterest && project.utmZone != 0) {
                    roiReproject()
                    reCentreMap()
                }
                inUTM = !inUTM
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
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
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
            font.family: mainWindow.headingFontStyle
            font.pixelSize: mainWindow.headingFontSize
            text: project.utmNorthSouthStr
            onClicked: {
                if (project.utmNorthSouth != 1) {
                    project.utmNorthSouthStr = "N"
                    project.utmNorthSouth = 1
                } else {
                    project.utmNorthSouthStr = "S"
                    project.utmNorthSouth = 0
                }
            }
        }
    }
    Text {
        id: size
        anchors.top: utmSelect.bottom
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        width: 200
        font.family: mainWindow.headingFontStyle
        font.pixelSize: mainWindow.headingFontSize
        color: "#000000"
        text: "Approx. Dimensions (m) = " + (project.maxEasting - project.minEasting).toFixed(0) + " x " + (project.maxNorthing - project.minNorthing).toFixed(0)
    }
    Rectangle {
        id: sampleSize
        anchors.top: size.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 2
        Text {
            id: sampleDist
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 2
            horizontalAlignment: Text.AlignRight
//            anchors.horizontalCenter: parent.horizontalCenter
            height: 20
            width: parent.width / 2
            font.family: mainWindow.headingFontStyle
            font.pixelSize: mainWindow.headingFontSize
            color: "#000000"
            text: "Sample spacing (m) "
        }
        Rectangle {
            id: sampleSizeEditRect
//            anchors.right: parent.right
            anchors.top: parent.top
            anchors.left: sampleDist.right
            anchors.margins: 2
            height: 20
            width: parent.width / 5
            color: "white"
            border.width: 1
            TextInput {
                id: sampleSizeEdit
                anchors.fill: parent
                anchors.margins: 2
                horizontalAlignment: Text.AlignHCenter
                validator: RegExpValidator { regExp : /[1-9][0-9]+/ }
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                color: "#000000"
                text: project.sampleSpacing
            }
        }
    }
    Text {
        anchors.top: sampleSize.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 2
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: "Add depth limits here!"
    }

    Component.onCompleted: reCentreMap()
}
