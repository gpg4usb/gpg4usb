import QtQuick 1.1


//FocusScope {
Rectangle {
    //width: 1600   //these are the only explicit sizes set
    //height: 1200   //all others are relative

    id: keydetails

    signal clicked

    anchors.fill: parent

    property alias tf1Text: tf1.text
    property alias tf2Text: tf2.text

//    property keyid : ""
//    property email : ""
//    property name : ""

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

     KeyInfoRow {
         x: 119
         y: 54
         // qsTr() for internationalisation, like tr()
         key: qsTr("id:")
         value: id
     }

     KeyInfoRow {
         x: 119
         y: 75
         key: qsTr("email:")
         value: email
     }

     KeyInfoRow {
         x: 119
         y: 96
         key: qsTr("name:")
         value: name
     }

     KeyInfoRow {
         x: 119
         y: 117
         key: qsTr("keysize")
         value: key.size() / key.encyptionSize()
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

     TextField {
         x: 119
         y: 155
         id: tf1
         text: "some text"
     }

    /* Button {
         x: 246
         y: 155
         text: "ok"
         onClicked: {
             console.log("ok clicked, text: " +  tf1.text)
         }
     }*/

     TextField {
         x: 119
         y: 188
         id: tf2
         text: "some other text"
     }

     Button {
         x: 246
         y: 188
         text: "ok"
         onClicked: {
            console.log("ok clicked, text: " +  tf1.text)
            console.log("tf2: " + tf2.text)
            keydetails.clicked();
         }
     }




}
