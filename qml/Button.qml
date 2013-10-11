import QtQuick 1.0

 Rectangle {
     id: container

     property variant text
     signal clicked

     height: text.height + 10; width: text.width + 20
     border.width: 1
     radius: 4
     smooth: true

     gradient: Gradient {
         GradientStop {
             position: 0.0
             color: !mouseArea.pressed ? activePalette.light : activePalette.button
         }
         GradientStop {
             position: 1.0
             color: !mouseArea.pressed ? activePalette.button : activePalette.dark
         }
     }

     SystemPalette { id: activePalette }

     MouseArea {
         id: mouseArea
         anchors.fill: parent
         onClicked: container.clicked()
     }

     Text {
         id: text
         anchors.centerIn:parent
         font.pointSize: 10
         text: parent.text
         color: activePalette.buttonText
     }
 }

//import QtQuick 1.0

// Rectangle {
//     id: container

//     property string text: "Button"
//     property string image: ""

//     signal clicked

//     //width: buttonLabel.width + 5; height: buttonLabel.height + buttonImage.height + 5
//     width: 60
//     height: 60
//     border { width: 1; color: Qt.darker(activePalette.button) }
//     smooth: true
//     radius: 8

//     // color the button with a gradient
//     gradient: Gradient {
//         GradientStop {
//             position: 0.0
//             color: {
//                 if (mouseArea.pressed)
//                     return activePalette.dark
//                 else
//                     return activePalette.light
//             }
//         }
//         GradientStop { position: 1.0; color: activePalette.button }
//     }

//     MouseArea {
//         id: mouseArea
//         anchors.fill: parent
//         onClicked: container.clicked();
//     }

//     Image {
//         id: buttonImage
//         y: 5
//         source: container.image
//         height: 30
//         width: 30
//         anchors.horizontalCenter: parent.horizontalCenter
//         //anchors.fill: parent
//     }

//     Text {
//         id: buttonLabel
//         //anchors.centerIn: container
//         anchors { top: buttonImage.bottom; horizontalCenter: parent.horizontalCenter }
//         color: activePalette.buttonText
//         text: container.text
//         horizontalAlignment: Text.AlignLeft
//     }
// }
