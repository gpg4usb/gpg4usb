import QtQuick 1.1

Rectangle {
    width: 400   //these are the only explicit sizes set
    height: 400   //all others are relative

    anchors.fill: parent

    //color: "red"
     Text {
         // qsTr() for internationalisation, like tr()
         text: qsTr("var:")
     }
     Text {
         x: 54
         y: 0
         text: var1
     }
}
