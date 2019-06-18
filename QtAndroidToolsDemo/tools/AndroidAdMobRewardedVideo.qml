
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    QtAndroidAdMobRewardedVideo {
        id: rewardedVideo
        unitId: "ca-app-pub-3940256099942544/5224354917"
        //onLoading: interstitialState.text = "Loading"
        //onLoaded: interstitialState.text = "Loaded"
        //onLoadError: interstitialState.text = "Error " + errorId
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
            text: "Rewarded video status"
        }
        Label {
            id: rewardedVideoState
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13
            text: "Not loaded"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Load rewarded video"
            onClicked: rewardedVideo.load();
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show rewarded video"
            onClicked: rewardedVideo.show();
        }
    }
}
