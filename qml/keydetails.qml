import QtQuick 1.1


//FocusScope {
Rectangle {
    //width: 1600   //these are the only explicit sizes set
    //height: 1200   //all others are relative

    id: keydetails

    signal clicked

    signal exportPrivateKeyClicked
    signal exportPublicKeyClicked

    anchors.fill: parent

    //property alias tf1Text: tf1.text
    //property alias tf2Text: tf2.text
    property string keyid: keymap.id

    Rectangle {
        id: rectangle2
        x: 0
        y: 0
        width: 1600
        height: 47
        color: "#99bff1"

        Text {
            id: text5
            x: 117
            y: 8
            text: qsTr("Keydetails")
            font.pixelSize:22
        }

        Text {
            id: text3
            x: 247
            y: 8
            text: qsTr("expired")
            font.italic: true
            font.pixelSize: 22
            visible: keymap.expired
            color: "#ff0000"
        }


        Text {
            id: text4
            x: 287
            y: 8
            text: qsTr("revoked")
            font.italic: true
            font.pixelSize: 22
            visible: keymap.revoked
            color: "#ff0000"
        }

    }

    Image {
        id: image1
        x: 0
        y: 0
        width: 107
        height: 229
        source: "qrc:/wizard_keys.png"
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
        id: text1
        x: 118
        y: 57
        text: qsTr("Owner details")
        font.bold: true
        font.pixelSize: 12
    }


    KeyInfoRow {
        x: 144
        y: 79
        key: qsTr("Name:")
        value: keymap.name
    }

    KeyInfoRow {
        x: 144
        y: 103
        key: qsTr("Email address:")
        value: keymap.email
    }

    KeyInfoRow {
        id: comment
        x: 144
        y: 126
        key: qsTr("Comment:")
        value: keymap.comment
    }

    Text {
        id: text2
        x: 118
        y: 166
        text: qsTr("Keydetails")
        font.pixelSize: 12
        font.bold: true
    }

    KeyInfoRow {
        x: 144
        y: 189
        key: qsTr("Key size:")
        value: keymap.size + "/" + keymap.encryptionSize
    }

    KeyInfoRow {
        id: expiration
        x: 144
        y: 212
        key: qsTr("Expires on:")
        value: (keymap.expirationDate != "") ? keymap.expirationDate : qsTr("Never")
    }

    KeyInfoRow {
        id: type
        x: 144
        y: 235
        key: qsTr("Algorithm:")
        value: keymap.algorithm + "/" + keymap.encryptionAlgorithm
    }

    KeyInfoRow {
        id: creation
        x: 144
        y: 258
        key: qsTr("Created on:")
        value: keymap.creationDate
    }

    KeyInfoRow {
         x: 144
         y: 281
         key: qsTr("Key ID:")
         value: keymap.id
     }

     Text {
         id: fingerprintLabel
         x: 118
         y: 329
         text: qsTr("Fingerprint")
         font.pixelSize: 12
         font.bold: true
     }

     TextEdit {
         id: fingerprint
         x: 144
         y: 350
         text: keymap.fingerprint
         font.pixelSize: 12
         readOnly: true
     }

     Button {
         id: exportPublicKeyButton
         x: 144
         y: 392
         text: qsTr("Export public key")
         onClicked: {
            keydetails.exportPublicKeyClicked();
         }

     }

     Button {
         id: exportPrivateKeyButton
         x: 367
         y: 392
         text: qsTr("Export private key")
         image: "qrc:/export_key_to_file.png"
         onClicked: {
            keydetails.exportPrivateKeyClicked();
         }
         visible: keymap.isSecret
     }

     Button {
         id: copyFingerprintButton
         x: fingerprint.x + fingerprint.width + 10
         y: 341
         //text: qsTr("copy")
         image: "qrc:/button_copy.png"
         onClicked: {
             fingerprint.selectAll();
             fingerprint.copy();
             fingerprint.deselect();
         }
     }


     /* some test stuff, should become change pw */
/*     TextField {
         x: 237
         y: 651
         id: tf1
         text: "some text"
     }

     TextField {
         x: 237
         y: 684
         id: tf2
         text: "some other text"
     }

     Button {
         x: 364
         y: 684
         text: "ok"
         onClicked: {
            console.log("ok clicked, text: " +  tf1.text)
            console.log("tf2: " + tf2.text)
            keydetails.clicked();
         }
     }*/

}
