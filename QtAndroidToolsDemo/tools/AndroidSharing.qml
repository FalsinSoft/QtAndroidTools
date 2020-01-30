import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

ScrollablePage {
    id: page
    padding: 20

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
    }
}
