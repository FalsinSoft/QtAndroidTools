
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    Connections {
        target: QtAndroidGoogleAccount
        onSignedInAccountChanged: accountPhoto.source = "image://SignedInAccountPhoto"
        onSignedIn: {
            if(signInSuccessfully === true)
            {                
            }
        }
        onSignedOut: {
            accountPhoto.source = "";
        }
    }

    Column {
        width: parent.width * 0.9
        height: parent.height * 0.9
        anchors.centerIn: parent
        spacing: 5

        Label {
            font.pixelSize: 15
            text: "<b>Id:</b> " + QtAndroidGoogleAccount.signedInAccount.id
        }

        Label {
            font.pixelSize: 15
            text: "<b>DisplayName:</b> " + QtAndroidGoogleAccount.signedInAccount.displayName
        }

        Label {
            font.pixelSize: 15
            text: "<b>Email:</b> " + QtAndroidGoogleAccount.signedInAccount.email
        }

        Label {
            font.pixelSize: 15
            text: "<b>FamilyName:</b> " + QtAndroidGoogleAccount.signedInAccount.familyName
        }

        Label {
            font.pixelSize: 15
            text: "<b>GivenName:</b> " + QtAndroidGoogleAccount.signedInAccount.givenName
        }

        Label {
            font.pixelSize: 15
            text: "<b>Photo:</b> "
        }
        Image {
            id: accountPhoto
            width: 100
            height: 100
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "sigIn"
            onClicked: QtAndroidGoogleAccount.signIn()
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "sigIn select account"
            onClicked: QtAndroidGoogleAccount.signInSelectAccount()
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "signOut"
            onClicked: QtAndroidGoogleAccount.signOut()
        }
    }
}
