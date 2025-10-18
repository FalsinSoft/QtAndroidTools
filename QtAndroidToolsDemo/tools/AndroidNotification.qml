import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

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
            onClicked: {
                if(QtAndroidSystem.sdkVersion() >= 33)
                {
                    permission.notificationIndex = 1;
                    permission.requestPermission("android.permission.POST_NOTIFICATIONS")
                }
                else notification1.show()
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Cancel notification"
            onClicked: notification1.cancel()
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Notification 2"
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

        CheckBox {
            id: showProgressBar
            text: "Show progress bar"
            checked: false
            onCheckedChanged: {
                if(checked)
                {
                    notification2.progressBar.max = sliderBar.to;
                    notification2.progressBar.current = sliderBar.value;
                }
                else
                {
                    notification2.progressBar.max = 0;
                    notification2.progressBar.current = 0;
                }
                notification2.show();
            }
        }
        Slider {
            id: sliderBar
            enabled: showProgressBar.checked
            width: parent.width
            from: 0
            to: 100
            value: 0
            onValueChanged: {
                notification2.progressBar.current = sliderBar.value;
                notification2.show();
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show notification"
            onClicked: {
                if(QtAndroidSystem.sdkVersion() >= 33)
                {
                    permission.notificationIndex = 2;
                    permission.requestPermission("android.permission.POST_NOTIFICATIONS")
                }
                else notification2.show()
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Cancel notification"
            onClicked: notification2.cancel()
        }

        QtAndroidAppPermissions {
            id: permission
            onRequestPermissionsResults: function(results)
            {
                if(results[0].granted === true)
                {
                    if(notificationIndex == 1)
                        notification1.show();
                    else
                        notification2.show();
                }
            }
            property int notificationIndex
        }
    }
}
