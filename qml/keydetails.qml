import QtQuick 1.1

Rectangle {
    width: 1600   //these are the only explicit sizes set
    height: 1200   //all others are relative

    anchors.fill: parent

    //color: "red"
    Rectangle {
        id: rectangle2
        x: 0
        y: 0
        width: 1600
        height: 47
        color: "#99bff1"
    }

    Rectangle {
        id: rectangle1
        x: 0
        y: 0
        width: 107
        height: 1200
        color: "#978c79"
    }

    Text {
         // qsTr() for internationalisation, like tr()
         x: 118
         y: 54
         text: qsTr("id:")
     }
     Text {
         x: 172
         y: 54
         text: id
     }

     Text {
         id: text1
         x: 118
         y: 75
         text: qsTr("email:")
     }

     Text {
         id: text2
         x: 172
         y: 75
         text: email
     }

     Text {
         id: text3
         x: 119
         y: 96
         text: qsTr("name:")
     }

     Text {
         id: text4
         x: 172
         y: 96
         text: name
     }


     Image {
         id: image1
         x: 0
         y: 0
         width: 107
         height: 229
         source: "qrc:/wizard_keys.png"
     }

     Text {
         id: text5
         x: 118
         y: 7
         width: 20
         height: 15
         text: qsTr("Keydetails")
         font.pixelSize:22
     }


}
