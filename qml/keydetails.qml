import QtQuick 1.1

Rectangle {
    width: 600   //these are the only explicit sizes set
    height: 400   //all others are relative

    anchors.fill: parent

    //color: "red"
    Rectangle {
        id: rectangle2
        x: 120
        y: 0
        width: 480
        height: 47
        color: "#3779d5"
    }

    Rectangle {
        id: rectangle1
        x: 0
        y: 0
        width: 120
        height: 400
        color: "#827968"
    }

    Text {
         // qsTr() for internationalisation, like tr()
         x: 136
         y: 54
         text: qsTr("id:")
     }
     Text {
         x: 190
         y: 54
         text: id
     }

     Text {
         id: text1
         x: 136
         y: 75
         text: qsTr("email:")
     }

     Text {
         id: text2
         x: 190
         y: 75
         text: email
     }

     Text {
         id: text3
         x: 137
         y: 96
         text: qsTr("name:")
     }

     Text {
         id: text4
         x: 190
         y: 96
         text: name
     }


     Image {
         id: image1
         x: 0
         y: 0
         width: 120
         height: 273
         source: "qrc:/wizard_keys.png"
     }

     Text {
         id: text5
         x: 136
         y: 7
         width: 20
         height: 15
         text: qsTr("Keydetails")
         font.pixelSize:22
     }


}
