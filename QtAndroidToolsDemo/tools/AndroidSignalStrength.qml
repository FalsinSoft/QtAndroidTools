import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

Page {
    id: page
    padding: 40

    function updateSignalLevel()
    {
        switch(QtAndroidSignalStrength.signalLevel)
        {
            case QtAndroidSignalStrength.LEVEL_GREAT:
                signalType.text = "Great"
                break;
            case QtAndroidSignalStrength.LEVEL_GOOD:
                signalType.text = "Good"
                break;
            case QtAndroidSignalStrength.LEVEL_MODERATE:
                signalType.text = "Moderate"
                break;
            case QtAndroidSignalStrength.LEVEL_POOR:
                signalType.text = "Poor"
                break;
            case QtAndroidSignalStrength.LEVEL_NONE:
                signalType.text = "None"
                break;
        }
    }

    Component.onCompleted: updateSignalLevel()

    Connections {
        target: QtAndroidSignalStrength
        function onSignalLevelChanged()
        {
            updateSignalLevel();
        }
    }

    Column {
        anchors.fill: parent
        spacing: 20

        Label {
            font.bold: true
            font.pixelSize: 25
            text: "Signal Strength:"
        }
        Label {
            font.pixelSize: 23
            text: QtAndroidSignalStrength.signalStrength
        }

        Label {
            font.bold: true
            font.pixelSize: 25
            text: "Signal Type:"
        }
        Label {
            id: signalType
            font.pixelSize: 23
        }
    }
}
