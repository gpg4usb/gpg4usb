import QtQuick 1.1

Rectangle {

    id: keyinforow

    width: 200
    height: 23

    property variant key
    property variant value

    Text {
         id: keyText
         text: key
     }

     Text {
         id: valueText
         x: 150
         y: 0
         text: value
     }
}
