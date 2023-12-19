import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

Page {
    id: page
    padding: 20

    Connections {
        target: QtAndroidUserMessagingPlatform
        function onConsentFormRequestFailure(errorMessage)
        {
            consentFormRequestResult.text = "Error: " + errorMessage;
        }
        function onConsentFormDismissed(consentGathered, privacyOptionsRequired)
        {
            consentFormRequestResult.text = "Consent Gathered " + (consentGathered ? "OK" : "Failed");
            showPrivacyOptionsFormButton.visible = privacyOptionsRequired;
        }
    }

    Column {
        anchors.fill: parent
        spacing: 5

        Label {
            id: consentFormRequestResult
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 15
            text: "-----"
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "loadAndShowConsentFormIfRequired"
            onClicked: QtAndroidUserMessagingPlatform.loadAndShowConsentFormIfRequired(false)
        }

        Button {
            id: showPrivacyOptionsFormButton
            visible: false
            anchors.horizontalCenter: parent.horizontalCenter
            text: "showPrivacyOptionsForm"
            onClicked: QtAndroidUserMessagingPlatform.showPrivacyOptionsForm()
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "resetConsentInformation"
            onClicked: QtAndroidUserMessagingPlatform.resetConsentInformation()
        }
        Label {
            id: consentCanShowAds
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 15
            text: "-----"
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "consentCanShowAds"
            onClicked: consentCanShowAds.text = QtAndroidUserMessagingPlatform.consentCanShowAds() ? "Yes" : "No"
        }
        Label {
            id: consentCanShowPersonalizedAds
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 15
            text: "-----"
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "consentCanShowPersonalizedAds"
            onClicked: consentCanShowPersonalizedAds.text = QtAndroidUserMessagingPlatform.consentCanShowPersonalizedAds() ? "Yes" : "No"
        }
    }
}
