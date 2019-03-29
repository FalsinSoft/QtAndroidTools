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
            smallIconName: "qtatoolsdemo_notification_icon1"
            largeIconSource: ":/images/logo_falsinsoft.jpg"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show notification"
            onClicked: notification1.show("test title 1", "test content")
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Cancel notification"
            onClicked: notification1.cancel()
        }

        QtAndroidNotification {
            id: notification2
            channelName: "Notification channel 2"
            smallIconName: "qtatoolsdemo_notification_icon2"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show notification"
            onClicked: notification2.show("test title 2", "test content")
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Cancel notification"
            onClicked: notification2.cancel()
        }
    }
}
