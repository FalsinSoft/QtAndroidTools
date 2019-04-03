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
            text: "Notification 1"
            font.bold: true
        }
        QtAndroidNotification {
            id: notification1
            title: "Notification 1 title"
            text: "This is the notification 1 content"
            expandableText: "This is a very very very very very very very very very very long expandable content"
            channelName: "Notification channel 1"
            smallIconName: "qtatoolsdemo_notification_icon1"
            largeIconSource: ":/images/logo_falsinsoft.jpg"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show notification"
            onClicked: notification1.show()
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Cancel notification"
            onClicked: notification1.cancel()
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Notification 1"
            font.bold: true
        }
        QtAndroidNotification {
            id: notification2
            title: "Notification 2 title"
            text: "This is the notification 2 content"
            channelName: "Notification channel 2"
            smallIconName: "qtatoolsdemo_notification_icon2"
            largeIconSource: ":/images/logo_falsinsoft.jpg"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show notification"
            onClicked: notification2.show()
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Cancel notification"
            onClicked: notification2.cancel()
        }
    }
}
