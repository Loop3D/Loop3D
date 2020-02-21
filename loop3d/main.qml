import QtQuick 2.12
//import QtQuick.Window 2.13
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import loop3d.projectmanagement 1.0

Item {
    width: 1024
    height: 768
    id: mainWindow
    property int myBorders: 6
    property string version: "1.0 x 10^-9"
    property bool confirmOnQuit: false
    property bool hasFile: false
    property string headingFontStyle: "Arial"
    property int headingFontSize: 14
    property string defaultFontStyle: "Arial"
    property int defaultFontSize: 12
    MenuBar {
        id: menu
        height: 40
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        font.pointSize: 10
        padding: 1

        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&New...")
                shortcut: "Ctrl+N"
                onTriggered: {
                    project.clearProject()
                    dcTab.roiReproject()
                    hasFile = false
                    dcTab.lockRegionOfInterest = false
                }
            }
            Action {
                text: qsTr("&Open...")
                shortcut: "Ctrl+O"
                onTriggered: fileDialogOpen.open()
            }
            Action {
                text: qsTr("&Save")
                shortcut: "Ctrl+S"
                onTriggered: {
                    if (hasFile) {
                        project.saveProject()
                        notifyText.text = "Saved project to " + project.filename
                    } else fileDialogSave.open()
                }
            }
            Action {
                text: qsTr("Save &As...")
                shortcut: "Ctrl+A"
                onTriggered: fileDialogSave.open()
            }
            MenuSeparator { }
            Action {
                text: qsTr("&Close")
                shortcut: "Ctrl+R"
                onTriggered: {
                    console.log("Extents currently are " + project.minLatitude + " " + project.maxLatitude + " " + project.minLongitude + " " + project.maxLongitude)
                }
            }
            Action {
                text: qsTr("&Quit")
                shortcut: "Ctrl+Q"
                onTriggered: {
                    if (confirmOnQuit) messageDialogQuit.open()
                    else Qt.quit()
                }
            }
        }
        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
                onTriggered: helpDialog.open()
            }
        }
    }

    Rectangle {
        id: header
        height: 55
        anchors.top: menu.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        TabBar {
            id: bar
            height: parent.height
            anchors.fill: parent
            padding: 4
            TabButton { text: 'Data Collection' }
//            TabButton { text: 'Geological Knowledge' }
            TabButton { text: 'Geological Modelling' }
//            TabButton { text: 'Geophysical Modelling' }
//            TabButton { text: 'Post Processing' }
            TabButton { text: 'Data Viewer' }
            onCurrentIndexChanged: project.mainIndex = currentIndex
        }
    }

    StackLayout {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: footer.top
        currentIndex: bar.currentIndex
        DcTab { id: dcTab }
//        KnowledgeTab { id: knowledgeTab }
        GeologyTab { id: geologyTab }
//        GeophysTab { id: geophysTab }
//        PostProTab { id: postProTab }
        DataViewer { id: dataViewTab }
    }

    Rectangle {
        id: footer
        height: 30
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        Rectangle {
            anchors.fill: parent
            id: notify
            color: "#dddddd"
            height: 24
            Text {
                id: notifyText
                text: "Loading..."
                font.pointSize: 11
                padding: 4
            }
        }
    }

    // Quit interaction
    MessageDialog {
        id: messageDialogQuit
        title: "Going so soon?"
        icon: StandardIcon.Question
        text: "Quit? Are you sure?"
        standardButtons: StandardButton.No |StandardButton.Yes
        onYes: Qt.quit()
    }

//    onClosing: {
//        if (confirmOnQuit) {
//            close.accepted = false
//            onTriggered: messageDialogQuit.open()
//        }
//    }

    // Open Project Interaction
    FileDialog {
        id: fileDialogOpen
        title: "Select a Loop3D project to open"
        nameFilters: [ "Loop3D Project files (*.loop3d *.l3d)","All files(*)"]
        selectExisting: true
        selectMultiple: false
        onRejected: console.log("Open Project Action Cancelled")
        onAccepted: {
            // Load file here
            if (!project.loadProject(fileDialogOpen.fileUrl)) {
                console.log("Open Project Action - Open " + fileDialogOpen.fileUrl)
                notifyText.text = "Loaded project from " + fileDialogOpen.fileUrl
                dcTab.inUTM = project.inUtm
                dcTab.lockRegionOfInterest = true
                hasFile = true
                dcTab.roiReproject()
                dcTab.reCentreMap()
            } else {
                notifyText.text = "ERROR loading project " + fileDialogOpen.fileUrl
            }
        }
    }

    // Save Project Interaction
    FileDialog {
        id: fileDialogSave
        title: "Select a Loop project filename to save to"
        defaultSuffix: "loop3d"
        nameFilters: [ "Loop3D Project files (*.loop3d *.l3d)","All files(*)" ]
        selectExisting: false
        selectMultiple: false
        onRejected: console.log("Save Project Action Cancelled")
        onAccepted: {
            // Save file here
            if (!project.saveProject(fileDialogSave.fileUrl)) {
                hasFile = true
                console.log("Save Project Action - Save to " + fileDialogSave.fileUrl)
                notifyText.text = "Saved project to " + fileDialogSave.fileUrl
            }
        }
    }

    // About interaction
    MessageDialog {
        id: helpDialog
        title: "Loop3D - Version " + mainWindow.version
        icon: StandardIcon.Question
        text: "Loop3D\n\nVersion: " + mainWindow.version
    }
}
