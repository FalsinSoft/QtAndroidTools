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

        TextEdit {
            id: packageName
            anchors.horizontalCenter: parent.horizontalCenter
            text: "com.falsinsoft.recomposephotopuzzle"
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "openAppDetails"
            onClicked: QtAndroidPlayStore.openAppDetails(packageName.text)
        }

        TextEdit {
            id: developerName
            anchors.horizontalCenter: parent.horizontalCenter
            text: "FalsinSoft"
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "openDeveloperAppList"
            onClicked: QtAndroidPlayStore.openDeveloperAppList(developerName.text)
        }
    }
}
