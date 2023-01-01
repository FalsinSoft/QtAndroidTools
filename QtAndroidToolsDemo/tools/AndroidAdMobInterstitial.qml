import QtQuick
import QtQuick.Controls
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    QtAndroidAdMobInterstitial {
        id: interstitial
        unitId: "ca-app-pub-3940256099942544/1033173712"
        onLoading: interstitialState.text = "Loading"
        onLoaded: interstitialState.text = "Loaded"
        onLoadError: interstitialState.text = "Error " + errorId
    }

    Column {
        width: parent.wdith
        height: parent.height * 0.8
        anchors.centerIn: parent
        spacing: 20


        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 15
            text: "Interstitial status"
        }
        Label {
            id: interstitialState
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13
            text: "Not loaded"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Load interstitial"
            onClicked: interstitial.load();
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show interstitial"
            onClicked: interstitial.show();
        }
    }
}
