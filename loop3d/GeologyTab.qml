import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import loop3d.pythontext 1.0
import loop3d.eventmodel 1.0
import loop3d.lsconfig 1.0

Item {
    id: geologyTab
//    property bool useLavavu: false
    function calcPerms() { project.totalPermutations = eventList.calcPermutations() }

    function repaint() { mycanvas.requestPaint() }

    function refreshModels() {
        eventsModel.refreshModel()
        observationsModel.refreshModel()
    }

    function runModel() {
        project.saveProject()
//        project.useLavavu = useLavavu
        project.pythonErrors = ""
        pythonText.run(textArea.text,project.filename,"GeologyModel")
    }

    function finishedModel() {
        project.reloadProject()
        if (project.pythonErrors) {
            console.log(project.pythonErrors)
//            pythonErrorDialog.open()
        } else {
            bar.currentIndex = 4
            if (project.useLavavu) {
                notifyText.text = "Lavavu viewer currently not implemented"
                concole.log("Lavavu viewer currently not implemented")
            }
        }

    }

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
                    width: configGL.width - mainWindow.myBorders
                    height: 30

                    Text {
                        anchors.fill: parent
                        anchors.margins: mainWindow.myBorders
                        verticalAlignment: Text.AlignVCenter
                        color: isActive ? "#000000" : "#555555"
                        font.italic: isActive ? false : true
                        text: "Event " + eventID + "\t" + (type == 0 ? "Fault" : (type == 1 ? "Fold" : (type == 2 ? "Foliation" : (type == 0 ? "Discontinuity" : "Strata")))) + "\t Age: " + minAge.toFixed(1) + " - " + maxAge.toFixed(1) + "  \t" + name
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
                            calcPerms()
                            mycanvas.clearCanvas()
                            mycanvas.requestPaint()
                            eventsModel.sortEvents()
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
                property real ageScale: 40.0;
                property real ageWidth: 10.0;
                property real eventWidth: 20.0;
                property real rankWidth: 45;
                onPaint: {
                    clearCanvas();
                    var ctx = getContext("2d");
                    var globalMinAge = 5000.0;
                    var globalMaxAge = -5000.0;
                    var i = 0;
                    for (i = 0; i < eventsModel.rowCount(); i++) {
                        if (eventsModel.dataIndexed(i,"isActive")) {
                            var tmp = eventsModel.dataIndexed(i,"minAge");
                            if (tmp < globalMinAge) globalMinAge = tmp;
                            tmp = eventsModel.dataIndexed(i,"minAge");
                            if (tmp > globalMaxAge) globalMaxAge = tmp;
                        }
                    }
                    globalMinAge = Math.floor(globalMinAge);
                    globalMaxAge = Math.ceil(globalMaxAge);
                    ageScale = 240.0 / (globalMaxAge - globalMinAge);
                    var globalAgeRange = globalMaxAge - globalMinAge;
                    drawTimelineScale(ctx,globalMinAge,globalMaxAge);

                    for (i = 0; i < eventsModel.countPBlocks();i++) {
                        var perms = eventsModel.pBlockDataIndexed(i,"permutations");
                        if (perms > 1) {
                            ctx.save();
                            var pBlockMinAge = eventsModel.pBlockDataIndexed(i,"minAge");
                            var pBlockMaxAge = eventsModel.pBlockDataIndexed(i,"maxAge");
                            var maxRank = eventsModel.pBlockDataIndexed(i,"maxRank");
                            ctx.fillStyle = perms > 50 ? "#ef0000" :
                                            perms > 20 ? "#aa3333" :
                                            perms > 5  ? "#00efef" :
                                            "#00aa00";
                            ctx.translate(100-eventWidth,40.0+(pBlockMinAge-globalMinAge)*ageScale);
                            ctx.fillRect(0,0,rankWidth*maxRank+2*eventWidth,(pBlockMaxAge - pBlockMinAge) * ageScale);
                            ctx.restore();
                        }
                    }

                    for (i = 0; i < eventsModel.rowCount(); i++) {
                        ctx.save();
                        if (eventsModel.dataIndexed(i,"isActive")) {
                            var isSelected = (i === detailsView.currentIndex);
                            ctx.fillStyle =  isSelected ? "#ff49ffff" :
                                             eventsModel.dataIndexed(i,"type") === 1 ? "#49ef49" :
                                             eventsModel.dataIndexed(i,"type") === 0 ? "#ef4949" :
                                             "#aaaaaa";
                            var maxAge = eventsModel.dataIndexed(i,"maxAge");
                            var minAge = eventsModel.dataIndexed(i,"minAge");
                            var avgAge = (minAge+maxAge)/2;
                            ctx.translate(100+eventsModel.dataIndexed(i,"rank")*rankWidth,40.0+(avgAge-globalMinAge)*ageScale);
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
                }
                function drawTimelineScale(ctx,globalMinAge,globalMaxAge) {
                    ctx.save();
                    ctx.lineWidth = 2;
                    ctx.beginPath();
                    ctx.moveTo(50,40.0+ageScale);
                    for (var i=1.0+globalMinAge;i<1.0+globalMaxAge;i+=(globalMaxAge-globalMinAge)/7.0) {
                        ctx.lineTo(50,i*ageScale);
                        ctx.lineTo(51,i*ageScale);
                        ctx.lineTo(50,i*ageScale);
                    }
                    ctx.stroke();
                    ctx.fillStyle = "#000000";
                    ctx.textAlign = "center";
                    ctx.textBaseline = "middle";
                    for (var j=globalMinAge;j<globalMaxAge;j+=(globalMaxAge-globalMinAge)/7.0) {
//                        ctx.fillText(j+" Ma",30,j*ageScale);
                        ctx.fillText((j)+" -",30,(j+1)*ageScale);
                    }
                    ctx.restore();
                }
                function whichSelected(x,y) {
                    var globalMinAge = 5000.0;
                    var globalMaxAge = -5000.0;
                    var i = 0;
                    for (i = 0; i < eventsModel.rowCount(); i++) {
                        if (eventsModel.dataIndexed(i,"isActive")) {
                            var tmp = eventsModel.dataIndexed(i,"minAge");
                            if (tmp < globalMinAge) globalMinAge = tmp;
                            tmp = eventsModel.dataIndexed(i,"minAge");
                            if (tmp > globalMaxAge) globalMaxAge = tmp;
                        }
                    }
                    globalMinAge = Math.floor(globalMinAge);
                    globalMaxAge = Math.ceil(globalMaxAge);
                    var globalAgeRange = globalMaxAge - globalMinAge;
                    var dist = 1000.0;
                    var index = -1;
                    for (i=0;i<eventsModel.rowCount();i++) {
                        var age = y/ ageScale + globalMinAge - 40/ageScale;
                        var rank = ((x - 100) / rankWidth);
                        var avgAge = (eventsModel.dataIndexed(i,"minAge") + eventsModel.dataIndexed(i,"maxAge")) / 2.0;
                        if (eventsModel.dataIndexed(i,"isActive")
                                && age > avgAge - eventWidth / ageScale / 2
                                && age < avgAge + eventWidth / ageScale / 2
                                && rank > eventsModel.dataIndexed(i,"rank") - 0.5
                                && rank < eventsModel.dataIndexed(i,"rank") + 0.5
                                && Math.abs(age-avgAge) <dist) {
                            dist = Math.abs(age - avgAge);
                            index = i;
                        }
                    }
                    return index;
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
            Text {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                height: 20
                text: "Total Permutations: " + project.totalPermutations
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
                id: stackGL
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - 60
                width: parent.width
                currentIndex: gtDetailsBar.currentIndex

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
                                calcPerms()
                                mycanvas.clearCanvas()
                                detailsView.currentIndex = eventsModel.findEventByID(eventID)
                                eventsModel.sortEvents()
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
                                calcPerms()
                                mycanvas.clearCanvas()
                                detailsView.currentIndex = eventsModel.findEventByID(eventID)
                                eventsModel.sortEvents()
                            }
                            selectByMouse: true
                            text: detailsView.currentIndex >= 0 ? eventsModel.dataIndexed(detailsView.currentIndex,"maxAge").toPrecision(5) : ""
                        }
                    }
                }

                Rectangle {
                    id: glOptionsTab
                    color: "#bbbbbb"
                    Flickable {
                        anchors.fill: parent
                        contentHeight: glOptionsGrid.height
                        clip: true
                        GridLayout {
                            id: glOptionsGrid
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: 5
                            columns: 2
                            rowSpacing: 2
                            Label {
                                text: "Foliation Options"
                                font.bold: true
                                color: "black"
                                Layout.columnSpan: 2
                            }
                            Label { text: "Interpolator Type" }
                            ComboBox {
                                Layout.minimumWidth: 200
                                model: ["PLI","something else"]
                                flat: true
                                currentIndex: lsConfig.foliationInterpolator
                                onCurrentIndexChanged: lsConfig.foliationInterpolator = currentIndex
                            }
                            Label { text: "Number of Elements" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: lsConfig.foliationNumElements
                                onTextChanged: lsConfig.foliationNumElements = text
                                validator: IntValidator { bottom: 0; top: 1000000 }
                            }
                            Label { text: "Buffer" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: lsConfig.foliationBuffer
                                onTextChanged: lsConfig.foliationBuffer = text
                                validator: RegExpValidator { regExp : /-?[0-9]+\.[0-9]+/ }
                            }
                            Label { text: "Solver" }
                            ComboBox {
                                Layout.minimumWidth: 200
                                model: ["pyamg","cg","lu"]
                                flat: true
                                currentIndex: lsConfig.foliationSolver
                                onCurrentIndexChanged: lsConfig.foliationSolver = currentIndex
                            }
                            Label { text: "Damp" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: lsConfig.foliationDamp
                                onTextChanged: lsConfig.foliationDamp = text
                                validator: IntValidator { bottom: 0; top: 100 }
                            }
                            Label {
                                text: "Fault Options"
                                font.bold: true
                                color: "black"
                                Layout.columnSpan: 2
                            }
                            Label { text: "Interpolator Type" }
                            ComboBox {
                                Layout.minimumWidth: 200
                                model: ["FDI","something else"]
                                flat: true
                                currentIndex: lsConfig.faultInterpolator
                                onCurrentIndexChanged: lsConfig.faultInterpolator = currentIndex
                            }
                            Label { text: "Number of Elements" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: lsConfig.faultNumElements
                                onTextChanged: lsConfig.faultNumElements = text
                                validator: IntValidator { bottom: 0; top: 1000000 }
                            }
                            Label { text: "Data Region" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: lsConfig.faultDataRegion
                                onTextChanged: lsConfig.faultDataRegion = text
                                validator: RegExpValidator { regExp : /-?[0-9]+\.[0-9]+/ }
                            }
                            Label { text: "Solver" }
                            ComboBox {
                                Layout.minimumWidth: 200
                                model: ["pyamg","cg","lu"]
                                flat: true
                                currentIndex: lsConfig.faultSolver
                                onCurrentIndexChanged: lsConfig.faultSolver = currentIndex
                            }
                            Label { text: "CPW" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: lsConfig.faultCpw
                                onTextChanged: lsConfig.faultCpw = text
                                validator: IntValidator { bottom: 0; top: 100 }
                            }
                            Label { text: "NPW" }
                            TextField {
                                Layout.fillHeight: true
                                placeholderText: lsConfig.faultNpw
                                onTextChanged: lsConfig.faultNpw = text
                                validator: IntValidator { bottom: 0; top: 1000 }
                            }
                            Label { text: ""; Layout.columnSpan: 2; }
                            Label { text: ""; Layout.columnSpan: 2; }
                            Label { text: ""; Layout.columnSpan: 2; }
                            Label { text: ""; Layout.columnSpan: 2; }
                            Label { text: ""; Layout.columnSpan: 2; }
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
                                filename: "processLoopStructural.py"
                                Component.onCompleted: {
                                    textDocToHighlight = textArea.textDocument
                                    // Use select all and text addition to trigger immediate highlighting
                                    textArea.selectAll()
                                    textArea.text += " "
                                }
                            }
                        }
                    }
                }
            }
            TabBar {
                id: gtDetailsBar
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.left: parent.left
                padding: 4
                TabButton { text: 'Details' }
                TabButton { text: 'Options' }
                TabButton { text: 'Python Code' }
            }

            Button {
                id: runCode
                anchors.bottom: stackGL.bottom
                anchors.right: stackGL.right
                height: 80
                width: 140
                text: "Create Model"
                onPressed: {
                    if (!project.hasFilename()) fileDialogSave.open()
                    else {
                        project.useLavavu = false
                        runModel()
                    }
                }
            }
//            Button {
//                id: runCodeLavavu
//                anchors.bottom: stackGL.bottom
//                anchors.right: runCode.left
//                height: 80
//                width: 200
//                text: "Create Lavavu Model"
//                onPressed: {
//                    if (!project.hasFilename()) fileDialogSave.open()
//                    else {
//                        project.useLavavu = true
//                        runModel()
//                    }
//                }
//            }
        }
    }
}
