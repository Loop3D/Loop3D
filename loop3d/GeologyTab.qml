import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import loop3d.pythontext 1.0

Item {
    id: geologyTab

    Rectangle {
        border.color: "#ffffff"
        border.width: 0
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#f0f0a1"
            }
            GradientStop {
                position: 1
                color: "#82820f"
            }
        }
        anchors.fill: parent

        Rectangle {
            id: configGL
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#dddddd"
            Text {
                anchors.fill: parent
                text: "List of features (selectable)\n QTreeView??"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: visualisationGL
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#999999"
            Text {
                anchors.fill: parent
                text: "Connected Graph of Features in sequence"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            id: detailsGL
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height
            color: "#bbbbbb"

            StackLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - 60
                width: parent.width
                currentIndex: detailsBar.currentIndex
                Rectangle {
                    id: codeTab
                    color: "#bbbbbb"

                    ScrollView {
                        id: sv
                        anchors.fill: parent
                        anchors.margins: 4
                        clip: true
                        TextEdit {
                            anchors.fill: parent
                            id: textArea
                            text: pythonText.pythonCode
                            selectByMouse: true
                            PythonText {
                                id: pythonText
                                filename: "..\\Notebook1ClaudiusData.py"
                                Component.onCompleted: {
                                    textDocToHighlight = textArea.textDocument
                                    // Use select all and text addition to trigger immediate highlighting
                                    textArea.selectAll()
                                    textArea.text += " "
                                }
                            }
                        }
                    }
                    Button {
                        id: runCode
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                        height: 80
                        width: 140
                        text: "Run Code"
                        onPressed: {
                            // TODO: remove file open when finished testing textures
                            if (!hasFile) fileDialogOpen.open()
                            else pythonText.run(textArea.text,project.filename)
                        }
                    }
                }

                Rectangle {
                    id: detailsTab
                    color: "#bbbbbb"
                    Text {
                        anchors.fill: parent
                        text: "Feature information (x,y,z,strike,dip)"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Rectangle {
                    id: resultsTab
                    color: "#bbbbbb"
                    Text {
                        anchors.fill: parent
                        text: "Once run visualisation of model"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

            }
            TabBar {
                id: detailsBar
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                padding: 4
                TabButton { text: 'Details' }
                TabButton { text: 'Python Code' }
                TabButton { text: 'Results' }
            }
        }
    }
}
