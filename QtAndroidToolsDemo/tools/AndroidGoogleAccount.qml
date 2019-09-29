
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    Connections {
        target: QtAndroidGoogleAccount
        onSignedIn: {
            if(signInSuccessfully === true)
            {
                accountPhoto.source = "image://LastSignedInAccountPhoto/";
            }
        }
    }

    Column {
        width: parent.width * 0.9
        height: parent.height * 0.9
        anchors.centerIn: parent
        spacing: 5

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "Id:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidGoogleAccount.lastSignedInAccount.id
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "DisplayName:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidGoogleAccount.lastSignedInAccount.displayName
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "Email:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidGoogleAccount.lastSignedInAccount.email
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "FamilyName:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidGoogleAccount.lastSignedInAccount.familyName
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "GivenName:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidGoogleAccount.lastSignedInAccount.givenName
        }

        Image {
            id: accountPhoto
            width: 200
            height: 200
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "sigIn"
            onClicked: QtAndroidGoogleAccount.signIn(true)
        }
    }
}
