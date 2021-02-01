import QtQuick 2.14
//import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Dialogs 1.2
import loop3d.projectmanagement 1.0
import loop3d.eventmodel 1.0
import loop3d.observationmodel 1.0
import loop3d.m2lconfig 1.0
import QtQuick.Scene3D 2.14
//import Qt3D.Input 2.14

Item {
    width: 1024
    height: 768
    id: mainWindow
    property int myBorders: 6
    property string version: "0.0.5"
    property bool confirmOnQuit: false
    property string headingFontStyle: "Arial"
    property int headingFontSize: 14
    property string defaultFontStyle: "Arial"
    property int defaultFontSize: 12
    Scene3D {
        id: mainScene3D
        anchors.fill: parent
        compositingMode: Scene3D.FBO
    }
    Isosurface { id: isosurface }

    EventModel {
        id: eventsModel
        events: eventList
    }
    ObservationModel {
        id: observationsModel
        observations: observationList
    }

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
                    project.clearProject(true)
                    dcTab.roiReproject()
                    project.setLockedExtents(false)
                    dmTab.reloadMap()
                }
            }
            Action {
                text: qsTr("&Open...")
                shortcut: "Ctrl+O"
                onTriggered: fileDialogOpen.open()
            }
            Action {
                text: qsTr("&Reload...")
                shortcut: "Ctrl+R"
                onTriggered: if (project.hasFilename()) project.reloadProject()
            }
            Action {
                text: qsTr("&Save")
                shortcut: "Ctrl+S"
                onTriggered: {
                    if (project.hasFilename()) {
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
                shortcut: "Ctrl+W"
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
            TabButton { text: 'Data Review' }
//            TabButton { text: 'Geological Knowledge' }
            TabButton { text: 'Geological Modelling' }
            TabButton { text: 'Geophysical Modelling' }
//            TabButton { text: 'Post Processing' }
            TabButton { text: 'Data Viewer' }
            onCurrentIndexChanged: {
                project.mainIndex = currentIndex
                if (currentIndex == 4) dataViewTab.forceActiveFocus()
            }
        }
    }

    StackLayout {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: footer.top
        currentIndex: bar.currentIndex
        DataColTab { id: dcTab }
        DataRevTab { id: dmTab }
//        KnowledgeTab { id: knowledgeTab }
        GeologyTab { id: geologyTab }
        GeophysTab { id: geophysTab }
//        PostProTab { id: postProTab }
        DataViewer { id: dataViewTab }
    }

    Rectangle {
        id: footer
        visible: project.flowChoiceMade
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

    Rectangle {
        id: flowChoiceArea
        visible: !project.flowChoiceMade
        anchors.top: menu.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#dddddd"
        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: false
            hoverEnabled: false
            preventStealing: true
        }
        Rectangle {
            id: flowArea
            width: parent.width * 2/3
            height: parent.height * 2/3
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color: "transparent"

            Button {
                width: 50
                height: 100
                anchors.right: flowArea.left
                anchors.verticalCenter: loopStructuralFlow.verticalCenter
                anchors.margins: 10
                text: "<"
                onClicked: {
                    project.decrementFlowChoice()
                }
            }
            Button {
                width: 50
                height: 100
                anchors.left: flowArea.right
                anchors.verticalCenter: loopStructuralFlow.verticalCenter
                anchors.margins: 10
                text: ">"
                onClicked: {
                    project.incrementFlowChoice()
                }
            }
            Rectangle {
                width: 500
                height: 100
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#00000000"
                anchors.margins: 100
                Button {
                    width: 200
                    height: 100
                    enabled: project.flowChoice
                    text: "Create New Work-flow"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    onClicked: project.flowChoiceMade = true
                }
                Button {
                    width: 200
                    height: 100
                    text: "Load Existing Project"
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    onClicked: fileDialogOpen.open()
                }
            }

            Rectangle {
                id: loopStructuralFlow
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: parent.height / 3
                color: project.flowChoice === 1 ? "lightblue" : "white"
                radius: 5
                border.width: 2
                border.color: "black"
                clip: true
                MouseArea {
                    anchors.fill: parent
                    onClicked: project.flowChoice = 1
                }
                Image {
                    id: curvedArrow
                    anchors.top: project.loopStructuralFlowOption === 0 ? parent.top : parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 5
                    width: parent.width
                    height: project.loopStructuralFlowOption < 2 ? parent.height : 0
                    source: "qrc:images/curvedArrow.png"
                    transform: Scale { yScale: project.loopStructuralFlowOption === 0? 1 : -1 }
                }
                Image {
                    id: doubleCurvedArrow
                    anchors.top: project.loopStructuralFlowOption === 2 ? parent.top : parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 5
                    height: project.loopStructuralFlowOption < 2 ? 0 : parent.height
                    source: "qrc:images/doubleCurvedArrow.png"
                    transform: Scale { yScale: project.loopStructuralFlowOption === 2 ? 1 : -1 }
                }

                Text {
                    text: "LoopStructural Flow"
                    anchors.top: parent.top
                    anchors.topMargin: 8
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                }
                Rectangle {
                    width: parent.width / 4
                    height: parent.height / 4
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.margins: 10
                    color: "pink"
                    border.width: 1
                    border.color: "black"
                    Text {
                        text: "Data Collection"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Rectangle {
                    width: parent.width / 4
                    height: parent.height / 4
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.margins: 10
                    color: "lightgreen"
                    border.width: 1
                    border.color: "black"
                    Text {
                        text: "Uncertainty Analysis"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Rectangle {
                    width: parent.width / 4
                    height: parent.height / 4
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.margins: 30
                    color: "deepskyblue"
                    border.width: 1
                    border.color: "black"
                    Text {
                        text: "Loop Structural Modelling"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Rectangle {
                    width: parent.width / 4
                    height: parent.height / 4
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.margins: 30
                    color: "yellow"
                    border.width: 1
                    border.color: "black"
                    Text {
                        text: "Geophysical Modelling"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
            Rectangle {
                id: gempyFlow
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: parent.height / 3
                color: project.flowChoice === 2 ? "lightblue" : "white"
                radius: 5
                border.width: 2
                MouseArea {
                    anchors.fill: parent
                    onClicked: project.flowChoice = 2
                }
                Text {
                    text: "Gempy Flow"
                    anchors.top: parent.top
                    anchors.topMargin: 8
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                }
                Image {
                    id: straightArrow
                    anchors.fill: parent
                    anchors.margins: 5
                    width: parent.width
                    height: parent.height
                    source: "qrc:images/straightArrow.png"
                    smooth: true
                }
                Rectangle {
                    width: parent.width / 4
                    height: parent.height / 4
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.margins: 10
                    color: "pink"
                    border.width: 1
                    border.color: "black"
                    Text {
                        text: "Data Collection"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Rectangle {
                    width: parent.width / 4
                    height: parent.height / 4
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.margins: 10
                    color: "lightgreen"
                    border.width: 1
                    border.color: "black"
                    Text {
                        text: "Uncertainty Analysis"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
                Rectangle {
                    width: parent.width / 4
                    height: parent.height / 4
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: "yellow"
                    border.width: 1
                    border.color: "black"
                    Text {
                        text: "Gempy Modelling"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
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
//                console.log("Open Project Action - Open " + fileDialogOpen.fileUrl)
                notifyText.text = "Loaded project from " + fileDialogOpen.fileUrl
                dcTab.inUTM = project.inUtm
                project.setLockedExtents(true)
                dcTab.roiReproject()
                dcTab.reCentreMap()
                project.flowChoiceMade = true
                geologyTab.repaint()
                geologyTab.calcPerms()
                viewer.resetView()
                viewer.resetCrossSection()
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
                project.setLockedExtents(true)
//                console.log("Save Project Action - Save to " + fileDialogSave.fileUrl)
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

    MessageDialog {
        id: pythonErrorDialog
        title: "An exception has been thrown by the python code"
        text: project.pythonErrors + '\nSee the console log for the stacetrace'
        icon: StandardIcon.Warning
    }
}
