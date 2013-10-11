import QtQuick 1.1

Rectangle {
    width: 400   //these are the only explicit sizes set
    height: 400   //all others are relative

    anchors.fill: parent

    //color: "red"
     Text {
         // qsTr() for internationalisation, like tr()
         text: qsTr("id:")
     }
     Text {
         x: 54
         y: 0
         text: id
     }

     Text {
         id: text1
         x: 0
         y: 21
         text: qsTr("email")
         font.pixelSize: 12
     }

     Text {
         id: text2
         x: 54
         y: 21
         text: email
         font.pixelSize: 12
     }

     Text {
         id: text3
         x: -4
         y: 38
         text: qsTr("name")
         font.pixelSize: 12
     }

     Text {
         id: text4
         x: 54
         y: 38
         text: name
         font.pixelSize: 12
     }
}
