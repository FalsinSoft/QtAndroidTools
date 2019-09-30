
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
                accountPhoto.source = "image://LastSignedInAccountPhoto";
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
            text: "<b>Id:</b> " + QtAndroidGoogleAccount.lastSignedInAccount.id
        }

        Label {
            font.pixelSize: 15
            text: "<b>DisplayName:</b> " + QtAndroidGoogleAccount.lastSignedInAccount.displayName
        }

        Label {
            font.pixelSize: 15
            text: "<b>Email:</b> " + QtAndroidGoogleAccount.lastSignedInAccount.email
        }

        Label {
            font.pixelSize: 15
            text: "<b>FamilyName:</b> " + QtAndroidGoogleAccount.lastSignedInAccount.familyName
        }

        Label {
            font.pixelSize: 15
            text: "<b>GivenName:</b> " + QtAndroidGoogleAccount.lastSignedInAccount.givenName
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
            text: "sigIn last account"
            onClicked: QtAndroidGoogleAccount.signIn()
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "sigIn new account"
            onClicked: QtAndroidGoogleAccount.signInNewAccount()
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "signOut"
            onClicked: QtAndroidGoogleAccount.signOut()
        }
    }
}
