import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

ScrollablePage {
    id: page
    padding: 20

    Column {
        width: parent.width * 0.9
        anchors.centerIn: parent
        spacing: 5

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "SDK version: " + QtAndroidSystem.sdkVersion()
            font.bold: true
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Request to send email"
            font.bold: true
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Request send"
            onClicked: QtAndroidSystem.requestEmailSend(["your@email.com"], "This is the subject", "This is the body", "Request to send email")
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Vibrate"
            onClicked: QtAndroidSystem.vibrate(250)
        }
    }
}
