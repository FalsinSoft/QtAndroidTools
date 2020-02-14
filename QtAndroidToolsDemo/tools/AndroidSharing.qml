import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.1
import QtAndroidTools 1.0

Page {
    id: page
    padding: 20

    Component.onCompleted: {
        if(QtAndroidSharing.action === QtAndroidSharing.ACTION_SEND)
        {
            if(QtAndroidSharing.mimeType === "text/plain")
            {
                receivedSharedText.text = QtAndroidSharing.getSharedText();
                receivedSharedText.open();
            }
            else if(QtAndroidSharing.mimeType.startsWith("image") === true)
            {
                QtAndroidTools.insertImage("SharedImage", QtAndroidSharing.getSharedData());
                sharedImage.source = "image://QtAndroidTools/SharedImage";
                receivedSharedImage.open();
            }
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
        TextField {
            id: sharedText
            width: parent.width
            text: "Hello Qt!"
            horizontalAlignment: TextField.AlignHCenter
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Share"
            onClicked: QtAndroidSharing.shareText(sharedText.text)
        }
    }

    MessageDialog {
        id: receivedSharedText
        title: "Received shared text"
        onAccepted: Qt.quit()
    }

    Dialog {
        id: receivedSharedImage
        title: "Received shared image"
        modal: true
        standardButtons: Dialog.Ok
        contentWidth: sharedImage.width
        contentHeight: sharedImage.height
        anchors.centerIn: parent

        Image {
            id: sharedImage
            width: page.width * 0.5
            height: width
        }

        onAccepted: Qt.quit()
    }
}
