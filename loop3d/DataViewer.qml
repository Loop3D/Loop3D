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
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: parent.width
        height: parent.height
    }
    MouseArea {
        id: viewerInteraction
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: mainWindow.myBorders
        width: parent.width
        height: parent.height
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
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#cccccc"
        Slider {
            id: isovalue
            from: 0
            value: 0
            to: 450
            width: parent.width - 100
            height: parent.height
            pressed: surface.isovalue = value

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
        }
        Button {
            anchors.left: isovalue.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 100
            onPressed: {
                surface.viewDistance = 20000.0
                surface.resetViewAngle()
            }
            text: "Reset View"
        }
    }
}
