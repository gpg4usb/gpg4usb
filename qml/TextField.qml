import QtQuick 1.1

Rectangle {

    id: textField

    property alias text: textInput.text

    width: 114
    height: 28
    color: "#ffffff"
    radius: 3
    border.width: 1
    border.color: "#999999"

    TextInput {
        id: textInput
        width: 102
        height: 20
        color: "#151515"
        text: parent.text
        selectionColor: "#008000"
        font.pixelSize: 12
        anchors.centerIn:parent
        onActiveFocusChanged: {
                if (textInput.activeFocus)
                    parent.border.color = "#0000ff";
                else
                    parent.border.color = "#999999";
            }
    }
}
