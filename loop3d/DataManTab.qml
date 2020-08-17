import QtQuick 2.14
import QtLocation 5.14
import QtPositioning 5.14
import QtQuick.Controls 2.14


Item {
    id: dmTab
    // Main area for data collection tab
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
            clip: true
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "List of Features (fault if a fault is selected, fold if ...,\n formation if ..., discontinuity if ...), could copy Geology Tab config area to begin with"
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
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "Table of Points of the feature selected\n (position, dip, dipdir, etc)"
            }
        }
        Rectangle {
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
