import QtQuick 2.14

Item {
    id: roiExtents

    function update(val) {
        editField.text = val
    }

    Text {
        id: extent
        text: parent.name
        anchors.top: highlightExtent.bottom
        anchors.left: parent.left
        horizontalAlignment: Text.AlignHCenter
        height: 20
        width: 90
        font.family: mainWindow.defaultFontStyle
        font.pixelSize: mainWindow.defaultFontSize
        color: "#000000"
        styleColor: "#aaaaaa"
    }
    Rectangle {
        id: highlightExtent
        anchors.left: parent.left
        anchors.top: parent.top
        height: 20
        width: 90
        color: "white"
        border.width: 1
        TextInput {
            id: editField
            readOnly: lockRegionOfInterest
            anchors.fill: parent
            color: "#000000"
            font.family: mainWindow.defaultFontStyle
            font.pixelSize: mainWindow.defaultFontSize
            validator: RegExpValidator { regExp : /-?[0-9]+\.[0-9]+/ }
            maximumLength: 10
            horizontalAlignment: Text.AlignHCenter
            Component.onCompleted: text = parent.parent.text
            onTextChanged: if (text != "" && text != "-" && !lockRegionOfInterest) parent.parent.text = text
            selectByMouse: true
            onFocusChanged: {
                if (inUTM) project.checkUTMLimits()
                else project.checkGeodeticLimits()
                roiReproject()
            }
        }
    }
}



/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
