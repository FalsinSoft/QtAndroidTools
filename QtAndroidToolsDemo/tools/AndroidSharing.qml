import QtQuick 2.12
import QtQuick.Controls 2.12
import QtAndroidTools 1.0

Page {
    id: page
    padding: 20

    Component.onCompleted: {
        if(QtAndroidSharing.action !== QtAndroidSharing.ACTION_NONE)
        {
            if(QtAndroidSharing.mimeType === "text/plain") receivedSharedtext.text = QtAndroidSharing.getSharedText();
        }
    }

    Column {
        anchors.fill: parent
        spacing: 20

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Text to share"
            font.bold: true
        }
        TextInput {
            id: sharedText
            width: parent.width
            text: "Hello Qt!"
            horizontalAlignment: TextInput.AlignHCenter
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Share"
            onClicked: QtAndroidSharing.shareText(sharedText.text)
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Received shared text"
            font.bold: true
        }
        Label {
            id: receivedSharedtext
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
