import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import loop3d.pythontext 1.0
import loop3d.eventmodel 1.0

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

        EventModel {
            id: eventsModel
            events: eventList
            Component.onCompleted: {
                eventList.appendItem(0,"Alpha",5.1,5.2,"Fold");
                eventList.appendItem(1,"Bravo",3.1,3.2,"Fault");
                eventList.appendItem(2,"Charlie",4.1,4.2,"Fold");
                eventList.appendItem(3,"Delta",1.1,2.2,"Fault");
                eventList.appendItem(4,"Echo",1.1,3.2,"Fold");
                eventList.appendItem(5,"Foxtrot",2.1,2.2,"Fold", 1);
                eventList.appendItem(6,"Golf",2.1,2.2,"Fault", 0, false);
                eventList.appendItem(7,"Hotel",2.1,2.2,"Fault", 2);
                eventList.appendItem(8,"India",2.1,2.2,"Fault", 0, false);
                eventList.appendItem(9,"Juliette",2.1,2.2,"Fold", 3);
                eventList.appendItem(900,"Kilo",6.1,6.2,"Fold");
                eventList.appendItem(901,"Lima",2.1,2.2,"Strata", 4);
                eventList.sort();
            }
        }

        Rectangle {
            id: configGL
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: mainWindow.myBorders
            width: parent.width / 2 - mainWindow.myBorders - 1
            height: parent.height / 2 - mainWindow.myBorders - 1
            color: "#dddddd"

            Text {
                id: configGLHeader
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.margins: mainWindow.myBorders
                text: "Event Log"
                height: 20
                font.bold: true
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignTop
            }

            ScrollView {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: configGLHeader.bottom
                clip: true
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ListView {
                    id: detailsView
                    anchors.fill: parent
                    anchors.margins: mainWindow.myBorders
                    model: eventsModel
                    delegate: eventsDelegate
                    highlight: Rectangle { height: 28; color: "#8849ffff"; radius: 20 }
                    currentIndex: -1
                    highlightFollowsCurrentItem: true
                    highlightMoveDuration: 200
                    highlightMoveVelocity: -1
                }
            }
            Component {
                id: eventsDelegate
                Item {
                    width: parent.width - mainWindow.myBorders
                    height: 30

                    Text {
                        anchors.fill: parent
                        anchors.margins: mainWindow.myBorders
                        verticalAlignment: Text.AlignVCenter
                        color: isActive ? "#000000" : "#555555"
                        font.italic: isActive ? false : true
                        text: "Event ID " + eventID + ": \t" + type + "\t Age: " + minAge.toFixed(1) + " - " + maxAge.toFixed(1) + "\t\t" + name
                        font.family: mainWindow.defaultFontStyle
                        font.pixelSize: mainWindow.defaultFontSize
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            detailsView.currentIndex = index
                            mycanvas.requestPaint()
                        }
                    }
                    Button {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: 80
                        text: isActive ? "Enabled" : "Disabled"
                        font.family: mainWindow.defaultFontStyle
                        font.pixelSize: mainWindow.defaultFontSize
                        background: Rectangle {
                            color: isActive ? "#3333ee" : "#ee3333"
                            radius: 5
                        }

                        onClicked: {
                            isActive = !isActive
                            eventList.sort()
                            mycanvas.clearCanvas()
                            mycanvas.requestPaint()
                        }
                    }
                }
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

            Canvas {
                id: mycanvas
                contextType: "2d"
                anchors.fill: parent
                property var ageScale: 40.0;
                property var ageWidth: 10.0;
                property var eventWidth: 20.0;
                property var rankWidth: 45;
                onPaint: {
                    var ctx = getContext("2d");

                    for (var i = 0; i < eventsModel.rowCount(); i++) {
                        ctx.save();
                        if (eventsModel.dataIndexed(i,"isActive")) {
                            var isSelected = (i === detailsView.currentIndex);
                            ctx.fillStyle =  isSelected ? "#ff49ffff" :
                                             eventsModel.dataIndexed(i,"type") === "Fold" ? "#49ef49" :
                                             eventsModel.dataIndexed(i,"type") === "Fault" ? "#ef4949" :
                                             "#aaaaaa";
                            var maxAge = eventsModel.dataIndexed(i,"maxAge");
                            var minAge = eventsModel.dataIndexed(i,"minAge");
                            var avgAge = (minAge+maxAge)/2;
                            ctx.translate(100+eventsModel.dataIndexed(i,"rank")*rankWidth,avgAge*ageScale);
                            ctx.beginPath();
                            ctx.moveTo(          0,(minAge-avgAge)*ageScale);
                            ctx.lineTo(-(ageWidth+eventWidth/2),(minAge-avgAge)*ageScale);
                            ctx.lineTo( (ageWidth+eventWidth/2),(minAge-avgAge)*ageScale);
                            ctx.lineTo(          0,(minAge-avgAge)*ageScale);
                            ctx.lineTo(          0,(maxAge-avgAge)*ageScale);
                            ctx.lineTo(-(ageWidth+eventWidth/2),(maxAge-avgAge)*ageScale);
                            ctx.lineTo( (ageWidth+eventWidth/2),(maxAge-avgAge)*ageScale);
                            ctx.lineTo(          0,(maxAge-avgAge)*ageScale);
                            ctx.stroke();
                            ctx.fillRect(-eventWidth/2,-eventWidth/2,eventWidth,eventWidth);
                            ctx.lineWidth = 2;
                            ctx.strokeStyle = "#000000";
                            ctx.strokeRect(-eventWidth/2,-eventWidth/2,eventWidth,eventWidth);
                            ctx.fillStyle = "#000000";
                            ctx.textAlign = "center";
                            ctx.fillText(eventsModel.dataIndexed(i,"eventID").toString(),0,0);
                        }
                        ctx.restore();
                    }
                    drawTimelineScale(ctx);
                }
                function drawTimelineScale(ctx) {
                    ctx.save();
                    ctx.lineWidth = 2;
                    ctx.beginPath();
                    ctx.moveTo(50,ageScale);
                    for (var i=1.0;i<8.0;i+=1.0) {
                        ctx.lineTo(50,i*ageScale);
                        ctx.lineTo(51,i*ageScale);
                        ctx.lineTo(50,i*ageScale);
                    }
                    ctx.stroke();
                    ctx.fillStyle = "#000000";
                    ctx.textAlign = "center";
                    ctx.textBaseline = "middle";
                    for (var j=1;j<8;j++) {
                        ctx.fillText(j+" Ma",30,j*ageScale);
                    }
                    ctx.restore();
                }
                function whichSelected(x,y) {
                    for (var i=0;i<eventsModel.rowCount();i++) {
                        var age = y / ageScale;
                        var rank = ((x - 100) / rankWidth);
                        var avgAge = (eventsModel.dataIndexed(i,"minAge") + eventsModel.dataIndexed(i,"maxAge")) / 2.0;
                        if (age > avgAge - eventWidth / ageScale / 2
                                && age < avgAge + eventWidth / ageScale / 2
                                && rank > eventsModel.dataIndexed(i,"rank") - 0.5
                                && rank < eventsModel.dataIndexed(i,"rank") + 0.5)
                            return i;
                    }
                    return -1;
                }
                function clearCanvas() {
                    var ctx = getContext("2d");
                    ctx.reset();
                    mycanvas.requestPaint();
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        detailsView.currentIndex = parent.whichSelected(mouseX,mouseY)
                        notifyText.text =  parent.whichSelected(mouseX,mouseY)
                        mycanvas.requestPaint()
                    }
                }
            }

            Text {
                anchors.fill: parent
                anchors.margins: mainWindow.myBorders
                text: "Timeline of Events"
                font.bold: true
                font.family: mainWindow.headingFontStyle
                font.pixelSize: mainWindow.headingFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignTop
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
                    id: detailsTab
                    color: "#bbbbbb"
                    Text {
                        id: headerGTDetails
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 15
                        anchors.margins: mainWindow.myBorders
                        text: "Feature information"
                        font.bold: true
                        font.family: mainWindow.headingFontStyle
                        font.pixelSize: mainWindow.headingFontSize
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignTop
                    }

                    Text {
                        id: featureNameAndType
                        anchors.top: headerGTDetails.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 30
                        anchors.margins: mainWindow.myBorders
                        visible: detailsView.currentIndex >= 0
                        text: "Feature: " + eventsModel.dataIndexed(detailsView.currentIndex,"name") + "\n" + eventsModel.dataIndexed(detailsView.currentIndex,"type")
                        font.family: mainWindow.defaultFontStyle
                        font.pixelSize: mainWindow.defaultFontSize
                    }
                    Rectangle {
                        id: minAgeTextInputRect
                        anchors.left: parent.left
//                        anchors.right: parent.right
                        anchors.top: featureNameAndType.bottom
                        anchors.margins: mainWindow.myBorders
                        height: 20
                        width: 80
                        color: "white"
                        border.width: 1
                        visible: detailsView.currentIndex >= 0
                        TextInput {
                            id: minAgeTextInput
                            anchors.fill: parent
                            color: "#000000"
                            font.family: mainWindow.defaultFontStyle
                            font.pixelSize: mainWindow.defaultFontSize
                            validator: RegExpValidator { regExp : /-?[0-9]+\.[0-9]+/ }
                            maximumLength: 10
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            onEditingFinished: {
                                var eventID = eventsModel.dataIndexed(detailsView.currentIndex,"eventID")
                                eventsModel.setDataIndexed(detailsView.currentIndex,text,"minAge")
                                eventsModel.sortEvents()
                                mycanvas.clearCanvas()
                                detailsView.currentIndex = eventsModel.findEventByID(eventID)
                            }
                            selectByMouse: true
                            text: detailsView.currentIndex >= 0 ? eventsModel.dataIndexed(detailsView.currentIndex,"minAge").toPrecision(5) : ""
                        }
                    }
                    Rectangle {
                        id: maxAgeTextInputRect
                        anchors.left: minAgeTextInputRect.right
                        anchors.top: featureNameAndType.bottom
                        anchors.margins: mainWindow.myBorders
                        height: 20
                        width: 80
                        color: "white"
                        border.width: 1
                        visible: detailsView.currentIndex >= 0
                        TextInput {
                            id: maxAgeTextInput
                            anchors.fill: parent
                            color: "#000000"
                            font.family: mainWindow.defaultFontStyle
                            font.pixelSize: mainWindow.defaultFontSize
                            validator: RegExpValidator { regExp : /-?[0-9]+\.[0-9]+/ }
                            maximumLength: 10
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            onEditingFinished: {
                                var eventID = eventsModel.dataIndexed(detailsView.currentIndex,"eventID")
                                eventsModel.setDataIndexed(detailsView.currentIndex,text,"maxAge")
                                eventsModel.sortEvents()
                                mycanvas.clearCanvas()
                                detailsView.currentIndex = eventsModel.findEventByID(eventID)
                            }
                            selectByMouse: true
                            text: detailsView.currentIndex >= 0 ? eventsModel.dataIndexed(detailsView.currentIndex,"maxAge").toPrecision(5) : ""
                        }
                    }

                }

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
//                                filename: ".\\Notebook1ClaudiusData.py"
                                filename: ".\\pickle_data.py"
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
                            else {
                                pythonText.run(textArea.text,project.filename)
                                detailsBar.currentIndex = 2
                            }
                        }
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
