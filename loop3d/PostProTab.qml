import QtQuick 2.12

Item {
    id: postProTab
    
    Rectangle {
        border.color: "#ffffff"
        border.width: 0
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#f0a1a1"
            }
            GradientStop {
                position: 1
                color: "#820f0f"
            }
        }
        anchors.fill: parent

        Rectangle {
            id: configPP
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#dddddd"
            Text {
                anchors.fill: parent
                text: "List of models with min/max/avg misfit (selectable)"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: visualisationPP
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#999999"
            Text {
                anchors.fill: parent
                text: "Visual representation individual models"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: detailsPP
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height
            color: "#bbbbbb"
            Text {
                anchors.fill: parent
                text: "More detail of combined models\nChange to Visualisation of uncertainty"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
