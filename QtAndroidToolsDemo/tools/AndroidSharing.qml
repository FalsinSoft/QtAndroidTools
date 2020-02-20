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
        else if(QtAndroidSharing.action === QtAndroidSharing.ACTION_PICK)
        {

        }
    }

    Connections {
        target: QtAndroidSharing
        onRequestedSharedFileReadyToGet: {
            requestedSharedFile.text = "Name: " + name + "\nSize: " + size + "\nMimeType: " + mimeType;
            requestedSharedFile.open();
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

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Share file"
            font.bold: true
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Share"
            onClicked: QtAndroidSharing.shareData("image/jpeg", QtAndroidSystem.dataLocation + "/sharedfiles/logo_falsinsoft.jpg")
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Request shared file"
            onClicked: QtAndroidSharing.requestSharedFile("image/*")
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

        property bool quitOnClose: true

        Image {
            id: sharedImage
            width: page.width * 0.5
            height: width
        }

        onAccepted: if(quitOnClose) Qt.quit()
    }

    MessageDialog {
        id: requestedSharedFile
        title: "It's ok to get this file?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onNo: QtAndroidSharing.closeSharedFile()
        onYes: {
            QtAndroidTools.insertImage("SharedImage", QtAndroidSharing.getRequestedSharedFile());
            sharedImage.source = "image://QtAndroidTools/SharedImage";
            receivedSharedImage.quitOnClose = false;
            receivedSharedImage.open();
        }
    }
}
