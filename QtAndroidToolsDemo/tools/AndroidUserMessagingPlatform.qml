import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

Page {
    id: page
    padding: 20

    Connections {
        target: QtAndroidUserMessagingPlatform
        function onConsentFormRequestResult(eventId)
        {
            switch(eventId)
            {
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_INFO_UPDATE_FAILURE:
                    consentFormRequestResult.text = "CONSENT_FORM_INFO_UPDATE_FAILURE";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_NOT_AVAILABLE:
                    consentFormRequestResult.text = "CONSENT_FORM_NOT_AVAILABLE";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_LOAD_SUCCESS:
                    consentFormRequestResult.text = "CONSENT_FORM_LOAD_SUCCESS";
                    break;
                case QtAndroidUserMessagingPlatform.CONSENT_FORM_LOAD_FAILURE:
                    consentFormRequestResult.text = "CONSENT_FORM_LOAD_FAILURE";
                    break;
            }
        }
        function onConsentFormClosed()
        {
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
            text: "requestConsentForm"
            onClicked: QtAndroidUserMessagingPlatform.requestConsentForm()
        }

        Label {
            id: consentStatus
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 15
            text: "-----"
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "consentStatus"
            onClicked: {
                var status = QtAndroidUserMessagingPlatform.consentStatus();

                switch(eventId)
                {
                    case QtAndroidUserMessagingPlatform.CONSENT_FORM_STATUS_UNKNOWN:
                        consentStatus.text = "CONSENT_FORM_STATUS_UNKNOWN";
                        break;
                    case QtAndroidUserMessagingPlatform.CONSENT_FORM_STATUS_REQUIRED:
                        consentStatus.text = "CONSENT_FORM_STATUS_REQUIRED";
                        break;
                    case QtAndroidUserMessagingPlatform.CONSENT_FORM_STATUS_NOT_REQUIRED:
                        consentStatus.text = "CONSENT_FORM_STATUS_NOT_REQUIRED";
                        break;
                    case QtAndroidUserMessagingPlatform.CONSENT_FORM_STATUS_OBTAINED:
                        consentStatus.text = "CONSENT_FORM_STATUS_OBTAINED";
                        break;
                }
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "showConsentForm"
            onClicked: QtAndroidUserMessagingPlatform.showConsentForm()
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "resetConsentInformation"
            onClicked: QtAndroidUserMessagingPlatform.resetConsentInformation()
        }
    }
}
