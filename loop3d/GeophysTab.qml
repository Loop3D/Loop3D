import QtQuick 2.12

Item {
    id: geophysTab
    
    Rectangle {
        border.color: "#ffffff"
        border.width: 0
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#a1f0a1"
            }
            GradientStop {
                position: 1
                color: "#0f820f"
            }
        }
        anchors.fill: parent

        Rectangle {
            id: configGF
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#dddddd"
            Text {
                anchors.fill: parent
                text: "Parafile to use\nSelectable data to toggle on/off"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: visualisationGF
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#999999"
            Text {
                anchors.fill: parent
                text: "Visual representation of grav/mag data or volume data"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: detailsGF
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height
            color: "#bbbbbb"
            Text {
                anchors.fill: parent
                text: "Modifiable Parafile\nModifiable Data\nChange to visualisation of Geophysical Model"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
