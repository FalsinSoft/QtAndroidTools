import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

Page {
    id: page
    padding: 0

    QtAndroidAdMobRewardedAd {
        id: rewardedAd
        unitId: "ca-app-pub-3940256099942544/5224354917"
        onRewarded: rewardedAdResult.text = "Type: " + type + " - Amount: " + amount
        onLoadError: rewardedAdState.text = "Error"
        onLoading: rewardedAdState.text = "Loading"
        onLoaded: rewardedAdState.text = "Loaded"
        onClicked: rewardedAdState.text = "Clicked"
        onDismissed: rewardedAdState.text = "Dismissed"
        onShowFailed: rewardedAdState.text = "Show failed"
        onImpression: rewardedAdState.text = "Impression"
        onShowed: rewardedAdState.text = "Showed"
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
            text: "Rewarded Ad status"
        }
        Label {
            id: rewardedAdState
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13
            text: "Not loaded"
        }
        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 15
            text: "Rewarded Ad result"
        }
        Label {
            id: rewardedAdResult
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13
            text: "No reward yet"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Load rewarded video"
            onClicked: rewardedAd.load();
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show rewarded Ad"
            onClicked: rewardedAd.show();
        }
    }
}
