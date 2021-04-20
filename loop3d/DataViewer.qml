import QtQuick 2.14
import QtQuick.Controls 2.14
import Qt3D.Core 2.14
import Qt3D.Input 2.14
import Qt3D.Render 2.14
import Qt3D.Extras 2.14
import QtQuick.Controls 2.14
import QtQuick.Scene3D 2.14


Item {
    MouseArea {
        id: viewerInteraction
        anchors.fill: parent

        // hover enables events without clicks
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property int lastX: -1
        property int lastY: -1
        property bool rotating: false
        property bool panning: false

        onWheel: {
            viewer.zoom(-wheel.angleDelta.y / 120.0)
        }

        onPressed: {
            if (mouse.button === Qt.LeftButton) {
                lastX = mouse.x
                lastY = mouse.y
                rotating = true
            }
            if (mouse.button === Qt.RightButton) {
                lastX = mouse.x
                lastY = mouse.y
                panning = true
            }
        }
        onPressAndHold: {
            if (mouse.button === Qt.LeftButton) {
                viewer.rotate(lastX-mouse.x,lastY-mouse.y)
                lastX = mouseX
                lastY = mouseY
                rotating = true
            }
            if (mouse.button === Qt.RightButton) {
                viewer.pan(lastX-mouse.x,lastY-mouse.y)
                lastX = mouseX
                lastY = mouseY
                panning = true
            }
        }
        onReleased: {
            if (mouse.button === Qt.LeftButton) {
                rotating = false
            }
            if (mouse.button === Qt.RightButton) {
                panning = false
            }
        }

        onPositionChanged: {
            if (rotating) {
                viewer.rotate(lastX-mouse.x,lastY-mouse.y)
                lastX = mouse.x
                lastY = mouse.y
            }
            if (panning) {
                viewer.pan(lastX-mouse.x,lastY-mouse.y)
                lastX = mouse.x
                lastY = mouse.y
            }
        }
        onExited: notifyText.text = ""
    }
    Keys.onPressed: {
        viewer.handleKeyEvent(event.key, event.modifiers)
    }
    // Main area for data viewer tab
    Rectangle {
        border.color: "#ffffff"
        border.width: 0
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#a1f0f0"
            }
            GradientStop {
                position: 1
                color: "#0f8282"
            }
        }
        Rectangle {
            id: mainDataViewerArea
            anchors.fill: parent
            anchors.margins: mainWindow.myBorders
            color: "#dddddd"
            ShaderEffectSource {
                id: mainShaderEffect
                anchors.fill: parent
                sourceItem: isosurface
            }
        }
        Rectangle {
            id: dataViewerControls
            property var isosurfaces: 1
            anchors.fill: parent
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: mainWindow.myBorders
            width: 10
            height: 100
            color: "transparent"
            ListModel {
                id: isovalueSliders
                ListElement { index: 0 }
                ListElement { index: 1 }
                ListElement { index: 2 }
                ListElement { index: 3 }
                ListElement { index: 4 }
                ListElement { index: 5 }
            }

            ListView {
                id: sliders
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.top: resetButton.bottom
                model: isovalueSliders
                delegate: isovalueDelegate
                interactive: false

            }

            Component {
                id: isovalueDelegate
                Slider {
                    id: isovalue
                    height: 30

                    visible: dataViewerControls.isosurfaces - 1 >= index
                    from: viewer.minScalarValue
                    value: (index+1) / 7 * (viewer.maxScalarValue - viewer.minScalarValue) + viewer.minScalarValue
                    to: viewer.maxScalarValue
                    pressed: {
                        viewer.setIsovalue(index,value)
                    }
                    Text {
                        anchors.fill: parent
                        text: viewer.miscToggle2 ? value : ""
                    }
                    Keys.onPressed: event.reject
                }
            }

            Button {
                id: lessIsovalues
                anchors.left: parent.left
                anchors.top: parent.top
                width: 45
                height: 40
                visible: dataViewerControls.isosurfaces > 1
                onPressed: {
                    if (dataViewerControls.isosurfaces > 1) {
                        dataViewerControls.isosurfaces = dataViewerControls.isosurfaces - 1
                        viewer.numIsosurfaces = dataViewerControls.isosurfaces
                    }
                }
                text: "-"
            }
            Button {
                id: moreIsovalues
                anchors.left: lessIsovalues.right
                anchors.top: parent.top
                width: 45
                height: 40
                visible: dataViewerControls.isosurfaces < 6
                onPressed: {
                    if (dataViewerControls.isosurfaces < 6) {
                        dataViewerControls.isosurfaces = dataViewerControls.isosurfaces + 1
                        viewer.numIsosurfaces = dataViewerControls.isosurfaces
                    }
                }
                text: "+"
            }
            Button {
                id: resetButton
                anchors.left: moreIsovalues.right
                anchors.top: parent.top
                width: 100
                height: 40
                onPressed: {
                    viewer.resetView()
                }
                text: "Reset View"
            }
    }
    Rectangle {
        id: crossSectionControls
        visible: viewer.miscToggle3
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: mainWindow.myBorders
        width: 200
        height: 150
        color: "transparent"
        Slider {
            id: crossSectionXSlider
            anchors.left: parent.left
            anchors.bottom: crossSectionYSlider.top
            height: 30
            width: 180
            from: viewer.structureXMin
            to: viewer.structureXMax
            value: (viewer.structureXMax + viewer.structureXMin) / 2.0
            pressed: viewer.csPositionX = value
            Text {
                anchors.fill: parent
                text: viewer.miscToggle2 ? crossSectionXSlider.value : ""
            }
            Keys.onPressed: event.reject
        }
        Slider {
            id: crossSectionYSlider
            anchors.left: parent.left
            anchors.bottom: crossSectionZSlider.top
            height: 30
            width: 180
            from: viewer.structureYMin
            to: viewer.structureYMax
            value: (viewer.structureYMax + viewer.structureYMin) / 2.0
            pressed: viewer.csPositionY = value
            Text {
                anchors.fill: parent
                text: viewer.miscToggle2 ? crossSectionYSlider.value : ""
            }
            Keys.onPressed: event.reject
        }
        Slider {
            id: crossSectionZSlider
            anchors.left: parent.left
            anchors.bottom: crossSectionPsiSlider.top
            height: 30
            width: 180
            from: (viewer.invertedView ? viewer.structureZMin : viewer.structureZMax )
            to: (viewer.invertedView ? viewer.structureZMax : viewer.structureZMin )
            value: (viewer.structureZMax + viewer.structureZMin) / 2.0
            pressed: viewer.csPositionZ = value
            Text {
                anchors.fill: parent
                text: viewer.miscToggle2 ? crossSectionZSlider.value : ""
            }
            Keys.onPressed: event.reject
        }
        Slider {
            id: crossSectionPsiSlider
            anchors.left: parent.left
            anchors.bottom: crossSectionThetaSlider.top
            height: 30
            width: 180
            from: -180.0
            to: 180.0
            value: 0
            pressed: viewer.csOrientationPsi = value
            Text {
                anchors.fill: parent
                text: viewer.miscToggle2 ? crossSectionPsiSlider.value : ""
            }
            Keys.onPressed: event.reject
        }
        Slider {
            id: crossSectionThetaSlider
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            height: 30
            width: 180
            from: -90.0
            to: 90.0
            value: 0.0
            pressed: viewer.csOrientationTheta = value
            Text {
                anchors.fill: parent
                text: viewer.miscToggle2 ? crossSectionThetaSlider.value : ""
            }
            Keys.onPressed: event.reject
        }
    }
    Rectangle {
            id: miscControls
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: mainWindow.myBorders
            width: 160
            color: "transparent"
            Button {
                id: miscButton1
                property var misc1: 0
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 4
                width: parent.width
                height: 40
                onPressed: {
                    misc1 = !misc1
                    viewer.miscToggle1 = misc1
                }
                text: "Options " + (viewer.miscToggle1 ? "ON" : "OFF")
            }
            Button {
                id: miscButton2
                property var misc2: 0
                visible: viewer.miscToggle1
                anchors.top: miscButton1.bottom
                anchors.right: parent.right
                anchors.margins: 4
                width: parent.width
                height: 40
                onPressed: {
                    misc2 = !misc2
                    viewer.miscToggle2 = misc2
                }
                text: "Labels " + (viewer.miscToggle2 ? "ON" : "OFF")
            }
            Button {
                id: miscButton3
                property var misc3: 0
                visible: viewer.miscToggle1
                anchors.top: miscButton2.bottom
                anchors.right: parent.right
                anchors.margins: 4
                width: parent.width
                height: 40
                onPressed: {
                    misc3 = !misc3
                    viewer.miscToggle3 = misc3
                    viewer.resetCrossSection()
                }
                text: "Slicing " + (viewer.miscToggle3 ? "ON" : "OFF")
            }
            Button {
                id: miscButton4
                property var misc4: 0
                visible: viewer.miscToggle1
                anchors.top: miscButton3.bottom
                anchors.right: parent.right
                anchors.margins: 4
                width: parent.width
                height: 40
                onPressed: {
                    misc4 = !misc4
                    viewer.invertedView = misc4
                    viewer.miscToggle4 = misc4
                }
                text: "Invert Depth " + (viewer.miscToggle4 ? "ON" : "OFF")
            }
            Button {
                id: miscButton5
                property var misc5: 0
                visible: viewer.miscToggle1
                anchors.top: miscButton4.bottom
                anchors.right: parent.right
                anchors.margins: 4
                width: parent.width
                height: 40
                onPressed: {
                    misc5 = !misc5
                    viewer.miscToggle5 = misc5
                }
                text: viewer.miscToggle5 ? "Strata Colour" : "Spectrum Colour"
            }
            Button {
                id: miscButton6
                property var misc6: 0
                visible: viewer.miscToggle1
                anchors.top: miscButton5.bottom
                anchors.right: parent.right
                anchors.margins: 4
                width: parent.width
                height: 40
                onPressed: {
                    misc6 = !misc6
                    viewer.miscToggle6 = misc6
                }
                text: viewer.miscToggle6 ? "Lighting Off" : "Lighting On"
            }
        }
    }
}
