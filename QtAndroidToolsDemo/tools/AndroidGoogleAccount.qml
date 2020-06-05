
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    Connections {
        target: QtAndroidGoogleAccount
        function onSignedInAccountInfoChanged()
        {
            QtAndroidTools.insertImage("AccountPhoto", QtAndroidGoogleAccount.signedInAccountInfo.photo);
            accountPhoto.source = "image://QtAndroidTools/AccountPhoto";
        }
        function onSignedIn(signInSuccessfully)
        {
            if(signInSuccessfully === true)
            {                
            }
        }
        function onSignedOut()
        {
            QtAndroidTools.removeImage("AccountPhoto");
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
            text: "<b>Id:</b> " + QtAndroidGoogleAccount.signedInAccountInfo.id
        }

        Label {
            font.pixelSize: 15
            text: "<b>DisplayName:</b> " + QtAndroidGoogleAccount.signedInAccountInfo.displayName
        }

        Label {
            font.pixelSize: 15
            text: "<b>Email:</b> " + QtAndroidGoogleAccount.signedInAccountInfo.email
        }

        Label {
            font.pixelSize: 15
            text: "<b>FamilyName:</b> " + QtAndroidGoogleAccount.signedInAccountInfo.familyName
        }

        Label {
            font.pixelSize: 15
            text: "<b>GivenName:</b> " + QtAndroidGoogleAccount.signedInAccountInfo.givenName
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
            onClicked: QtAndroidGoogleAccount.signIn(QtAndroidGoogleDrive.SCOPE_DRIVE)
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "sigIn select account"
            onClicked: QtAndroidGoogleAccount.signInSelectAccount(QtAndroidGoogleDrive.SCOPE_DRIVE)
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "signOut"
            onClicked: QtAndroidGoogleAccount.signOut()
        }
    }
}
