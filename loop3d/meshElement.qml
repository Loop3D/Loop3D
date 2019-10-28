import QtQuick 2.0
import loop3d.meshrenderer 1.0
//import Qt3D.Core 2.12
//import Qt3D.Render 2.12
//import Qt3D.Extras 2.12
//import QtQuick.Scene3D 2.13

Item {
    width: 320
    height: 480
    GriddedMesh {
        id: meshrender
//            anchors.top: parent.top
//            anchors.left: parent.left
//            anchors.margins: mainWindow.myBorders
//            width: parent.width / 2 - mainWindow.myBorders - 1
//            height: parent.height / 2 - mainWindow.myBorders - 1
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }
    }

    Rectangle {
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: "The background here is a squircle rendered with raw OpenGL using the 'beforeRender()' signal in QQuickWindow. This text label and its border is rendered using QML"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
    }
}
