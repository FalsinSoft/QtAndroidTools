import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools.BatteryState 1.0

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
