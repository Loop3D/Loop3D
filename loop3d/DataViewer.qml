import QtQuick 2.0
import QtQuick.Controls 2.12
import Qt3D.Core 2.12
import Qt3D.Input 2.12
import Qt3D.Render 2.12
import Qt3D.Extras 2.12
import OpenGLUnderQML 1.0
import QtQuick.Controls 2.12

Item {
    Isosurface {
        id: surface
        anchors.fill: parent

        Component.onCompleted: resetViewAngle()

        MouseArea {
            id: viewerInteraction
            anchors.fill: parent

            // hover enables events without clicks
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property int lastX: -1
            property int lastY: -1
            property bool panning: false

            onWheel: {
                surface.updateViewDistance(-wheel.angleDelta.y / 120.0)
            }

            onPressed: {
                if (mouse.button === Qt.LeftButton) {
                    lastX = mouse.x
                    lastY = mouse.y
                    panning = true
                }
            }
            onPressAndHold: {
                if (mouse.button === Qt.LeftButton) {
                    surface.updateViewAngle(mouse.x-lastX, mouse.y-lastY)
                    lastX = mouseX
                    lastY = mouseY
                    panning = true
                }
                if (mouse.button === Qt.RightButton) {
                }
            }
            onReleased: {
                if (mouse.button === Qt.LeftButton) {
                    panning = false
                }
            }

            onPositionChanged: {
                if (panning) {
                    surface.updateViewAngle(mouse.x-lastX, mouse.y-lastY)
                    lastX = mouse.x
                    lastY = mouse.y
                }

            }
            onExited: notifyText.text = ""
        }
    }

    Rectangle {
        id: dataViewerControls
        property var isosurfaces: 1
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 4
        width: 10
        color: "#00000000"

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

                visible: dataViewerControls.isosurfaces - 1 >= index
                from: surface.isovalueMin
                value: (index+1) / 7 * (surface.isovalueMax - surface.isovalueMin) + surface.isovalueMin
                to: surface.isovalueMax
                pressed: surface.updateIsovalue(index, value)

                states: State {
                    name: "expanded"
                    when: isExpanded
                    PropertyChanges {
                        target: sliderItem
                        width: itemWidth
                    }
                }

                transitions:[
                    Transition {
                        from: ""
                        to: "expanded"
                        reversible: true
                        SequentialAnimation {
                            PropertyAnimation { property: "width"; duration: 200 }
                        }
                    }
                ]

                background: Rectangle {
                    x: isovalue.leftPadding
                    y: isovalue.topPadding + isovalue.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 4
                    width: isovalue.availableWidth
                    height: implicitHeight
                    radius: 2
                    color: "#bdbebf"

                    Rectangle {
                        width: isovalue.visualPosition * parent.width
                        height: parent.height
                        color: "#21be2b"
                        radius: 2
                    }
                }

                handle: Rectangle {
                    x: isovalue.leftPadding + isovalue.visualPosition * (isovalue.availableWidth - width)
                    y: isovalue.topPadding + isovalue.availableHeight / 2 - height / 2
                    implicitWidth: 26
                    implicitHeight: 26
                    radius: 13
                    color: isovalue.pressed ? "#f0f0f0" : "#f6f6f6"
                    border.color: "#bdbebf"
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
                    surface.updateNumberOfIsosurfaces(dataViewerControls.isosurfaces)
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
                    surface.updateNumberOfIsosurfaces(dataViewerControls.isosurfaces)
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
                surface.viewDistance = 20000.0
                surface.resetViewAngle()
            }
            text: "Reset View"
        }
    }

    Keys.onPressed: {
        surface.updateCameraPosition(event.key === Qt.Key_Space,
                                     event.key === Qt.Key_Shift,
                                     event.key === Qt.Key_A || (event.key === Qt.Key_4 && (event.modifiers & Qt.KeypadModifier)),
                                     event.key === Qt.Key_D || (event.key === Qt.Key_6 && (event.modifiers & Qt.KeypadModifier)),
                                     event.key === Qt.Key_W || (event.key === Qt.Key_8 && (event.modifiers & Qt.KeypadModifier)),
                                     event.key === Qt.Key_S || (event.key === Qt.Key_2 && (event.modifiers & Qt.KeypadModifier)))
    }
}
