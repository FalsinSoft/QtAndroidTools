import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    Column {
        width: parent.wdith
        height: parent.height * 0.8
        anchors.centerIn: parent
        spacing: 20

        QtAndroidNotification {
            id: notification1
            channelName: "Notification channel 1"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show notification"
            onClicked: {
            }
        }
    }
}
