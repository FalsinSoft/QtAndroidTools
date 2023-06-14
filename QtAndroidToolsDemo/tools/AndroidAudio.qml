import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

Page {
    id: page
    padding: 40

    Column {
        width: parent.width * 0.9
        height: parent.height * 0.9
        anchors.centerIn: parent
        spacing: 15

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 15
            text: "Focus:"
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 15
            text: QtAndroidAudio.focus ? "Yes" : "No"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "requestFocus"
            onClicked: QtAndroidAudio.requestFocus()
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "abandonFocus"
            onClicked: QtAndroidAudio.abandonFocus()
        }
    }
}
