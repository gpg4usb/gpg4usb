import QtQuick 1.1

Rectangle {

    id: fileencryption

    signal okClicked
    signal fileChooserClicked

    property alias inputFilePath : inputFilePath.text
    property alias outputFilePath : outputFilePath.text
    property alias showNoKeySelected : noKeySelectInfo.visible

    width: 400
    height: 400

    Text {
        id: text3
        x: 21
        y: 16
        text: qsTr("Encrypt File")
        font.pixelSize: 12
    }

    Text {
        id: text1
        x: 56
        y: 65
        text: qsTr("Input:")
        font.pixelSize: 12
    }

    TextField {
        id: inputFilePath
        x: 146
        y: 65
        width: 150
        height: 28
    }

    Button {
        id: fileChooserButton
        x: 327
        y: 65
        text: "..."
        onClicked: {
            console.log("fc click")
            fileencryption.fileChooserClicked()
        }
    }

    Text {
        id: text2
        x: 56
        y: 104
        text: qsTr("Output:")
        font.pixelSize: 12
    }

    TextField {
        id: outputFilePath
        x: 146
        y: 104
        width: 150
        height: 28
    }

    Button {
        id: okButton
        x: 270
        y: 150
        width: 97
        height: 35
        text: qsTr("OK")
        onClicked: fileencryption.okClicked()
    }

    Rectangle {
        id: noKeySelectInfo
        x: 21
        y: 150
        width: 142
        height: 35
        color: "#ffffff"
        border.width: 2
        border.color: "#d95b1d"
        visible: false

        Text {
            id: text4
            x: 6
            y: -1
            text: qsTr("!")
            font.bold: true
            font.pointSize: 20
            font.pixelSize: 12
        }

        Text {
            id: text5
            x: 31
            y: 9
            text: qsTr("No Key Selected")
            font.pixelSize: 12
        }
    }


}
