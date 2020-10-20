import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 20

    Connections {
        target: QtAndroidUserMessagingPlatform
        function onConsentFormShowResult(eventId)
        {
            switch(eventId)
            {
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_INFO_UPDATE_FAILURE:
                    consentFormShowResult.text = "CONSENT_FORM_INFO_UPDATE_FAILURE";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_NOT_AVAILABLE:
                    consentFormShowResult.text = "CONSENT_FORM_NOT_AVAILABLE";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_STATUS_UNKNOWN:
                    consentFormShowResult.text = "CONSENT_FORM_STATUS_UNKNOWN";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_STATUS_REQUIRED:
                    consentFormShowResult.text = "CONSENT_FORM_STATUS_REQUIRED";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_STATUS_NOT_REQUIRED:
                    consentFormShowResult.text = "CONSENT_FORM_STATUS_NOT_REQUIRED";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_STATUS_OBTAINED:
                    consentFormShowResult.text = "CONSENT_FORM_STATUS_OBTAINED";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_LOAD_FAILURE:
                    consentFormShowResult.text = "CONSENT_FORM_LOAD_FAILURE";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_DISMISSED:
                    consentFormShowResult.text = "CONSENT_FORM_DISMISSED";
                    break;
            }
        }
    }

    Column {
        anchors.fill: parent
        spacing: 5

        Label {
            id: consentFormShowResult
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 15
            text: "-----"
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "showConsentFormIfRequired"
            onClicked: QtAndroidUserMessagingPlatform.showConsentFormIfRequired()
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "resetConsentInformation"
            onClicked: QtAndroidUserMessagingPlatform.resetConsentInformation()
        }
    }
}
