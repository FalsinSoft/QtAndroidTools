import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

Page {
    id: page
    padding: 40

    Column {
        anchors.fill: parent
        spacing: 20

        Label {
            font.bold: true
            font.pixelSize: 25
            text: "Battery Level:"
        }
        Label {
            id: batteryLevel
            font.pixelSize: 23
            text: QtAndroidBatteryState.level
        }
        Label {
            font.bold: true
            font.pixelSize: 25
            text: "Battery on charge:"
        }
        Label {
            id: batteryOnCharge
            font.pixelSize: 23
            text: QtAndroidBatteryState.onCharge
        }
    }
}
