import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

Page {
    id: page
    padding: 0

    Column {
        width: parent.width
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
            text: "SCREEN_ORIENTATION_REVERSE_LANDSCAPE"
            onClicked: QtAndroidScreen.setOrientation(QtAndroidScreen.SCREEN_ORIENTATION_REVERSE_LANDSCAPE)
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "SCREEN_ORIENTATION_PORTRAIT"
            onClicked: QtAndroidScreen.setOrientation(QtAndroidScreen.SCREEN_ORIENTATION_PORTRAIT)
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "SCREEN_ORIENTATION_REVERSE_PORTRAIT"
            onClicked: QtAndroidScreen.setOrientation(QtAndroidScreen.SCREEN_ORIENTATION_REVERSE_PORTRAIT)
        }

        CheckBox {
            text: qsTr("Keep screen on")
            onClicked: QtAndroidScreen.keepScreenOn(checkState === Qt.Checked)
        }
    }
}
