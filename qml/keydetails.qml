import QtQuick 1.1

Rectangle {
    width: 400   //these are the only explicit sizes set
    height: 400   //all others are relative

    anchors.fill: parent

    //color: "red"
     Text {
         // qsTr() for internationalisation, like tr()
         x: 118
         y: 37
         text: qsTr("id:")
     }
     Text {
         x: 172
         y: 37
         text: id
     }

     Text {
         id: text1
         x: 118
         y: 58
         text: qsTr("email:")
     }

     Text {
         id: text2
         x: 172
         y: 58
         text: email
     }

     Text {
         id: text3
         x: 119
         y: 79
         text: qsTr("name:")
     }

     Text {
         id: text4
         x: 172
         y: 79
         text: name
     }

     Image {
         id: image1
         x: 0
         y: 0
         width: 100
         height: 400
         source: "qrc:/wizard_keys.png"
     }

     Text {
         id: text5
         x: 119
         y: 0
         width: 20
         height: 15
         text: qsTr("Keydetails")
         font.pixelSize:22
     }
}
