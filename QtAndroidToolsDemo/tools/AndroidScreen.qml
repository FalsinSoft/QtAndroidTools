
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

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "SCREEN_ORIENTATION_LANDSCAPE"
            onClicked: QtAndroidScreen.setOrientation(QtAndroidScreen.SCREEN_ORIENTATION_LANDSCAPE)
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "SCREEN_ORIENTATION_PORTRAIT"
            onClicked: QtAndroidScreen.setOrientation(QtAndroidScreen.SCREEN_ORIENTATION_PORTRAIT)
        }
    }
}
