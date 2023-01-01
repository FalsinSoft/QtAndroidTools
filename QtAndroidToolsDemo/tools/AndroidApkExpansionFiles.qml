import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import QtAndroidTools 1.0

Page {
    id: page
    padding: 20

    function downloadInProgress(downloading)
    {
        buttonStartDownload.enabled = !downloading;
        buttonPauseDownload.enabled = downloading;
        buttonContinueDownload.enabled = downloading;
        buttonAbortDownload.enabled = downloading;
    }

    function downloadFiles()
    {
        QtAndroidApkExpansionFiles.base64PublicKey = "app_public_key_here";
        QtAndroidApkExpansionFiles.salt = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20];
        QtAndroidApkExpansionFiles.main.version = 1;
        QtAndroidApkExpansionFiles.main.size = 123456789;
        QtAndroidApkExpansionFiles.patch.version = 1;
        QtAndroidApkExpansionFiles.patch.size = 123456789;

        if(QtAndroidApkExpansionFiles.base64PublicKey === "")
        {
            errorMessageBox.text = "Invalid base64PublicKey";
            errorMessageBox.open();
            return;
        }

        switch(QtAndroidApkExpansionFiles.startDownloadFiles())
        {
            case QtAndroidApkExpansionFiles.APKEF_NO_DOWNLOAD_REQUIRED:
                fileDeliveredMessageBox.open();
                break;
            case QtAndroidApkExpansionFiles.APKEF_DOWNLOAD_STARTED:
            case QtAndroidApkExpansionFiles.APKEF_LVL_CHECK_REQUIRED:
                page.downloadInProgress(true);
                break;
            case QtAndroidApkExpansionFiles.APKEF_STORAGE_READ_PERMISSION_REQUIRED:
            case QtAndroidApkExpansionFiles.APKEF_STORAGE_WRITE_PERMISSION_REQUIRED:
                errorMessageBox.text = "Without storage access permission the app can not download expansion files";
                errorMessageBox.open();
                page.downloadInProgress(false);
                break;
            default:
                errorMessageBox.text = "Sorry, unknown error happened";
                errorMessageBox.open();
                page.downloadInProgress(false);
                break;
        }
    }

    Connections {
        target: QtAndroidApkExpansionFiles
        function onDownloadStateChanged(newState)
        {
            switch(newState)
            {
                case QtAndroidApkExpansionFiles.STATE_COMPLETED:
                    fileDeliveredMessageBox.open();
                    page.downloadInProgress(false);
                    break;
                case QtAndroidApkExpansionFiles.STATE_FAILED_UNLICENSED:
                case QtAndroidApkExpansionFiles.STATE_FAILED_FETCHING_URL:
                case QtAndroidApkExpansionFiles.STATE_FAILED_SDCARD_FULL:
                case QtAndroidApkExpansionFiles.STATE_FAILED_CANCELED:
                case QtAndroidApkExpansionFiles.STATE_FAILED:
                    errorMessageBox.text = QtAndroidApkExpansionFiles.getString(newState-1);
                    errorMessageBox.open();
                    page.downloadInProgress(false);
                    break;
            }
            downloadStateLabel.text = QtAndroidApkExpansionFiles.getString(newState-1);
        }
        function onDownloadProgress(overallTotal, overallProgress, timeRemaining, currentSpeed)
        {
            var time = new Date(timeRemaining);
            downloadProgressBar.to = overallTotal;
            downloadProgressBar.value = overallProgress;
            downloadSizeLabel.text = (overallProgress / (1024*1024)).toFixed(2) + "MB/" + (overallTotal / (1024*1024)).toFixed(2) + "MB";
            downloadTimeLabel.text = "Time left: ";
            if(timeRemaining > 1000 * 60 * 60)
                downloadTimeLabel.text += ("0" + time.getHours()).substr(-2) + ":" + ("0" + time.getMinutes()).substr(-2);
            else
                downloadTimeLabel.text += ("0" + time.getMinutes()).substr(-2) + ":" + ("0" + time.getSeconds()).substr(-2);
        }
    }

    Column {
        anchors.fill: parent
        spacing: 20

        Label {
            width: parent.width
            text: "Download apk expansion files"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Label.Wrap
            font.pixelSize: 14
            font.bold: true
        }

        ProgressBar {
            id: downloadProgressBar
            width: parent.width * 0.85
            anchors.horizontalCenter: parent.horizontalCenter
            from: 0
        }

        Label {
            id: downloadStateLabel
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Label.Wrap
            font.pixelSize: 15
        }

        Button {
            id: buttonStartDownload
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Start download"
            onClicked: page.downloadFiles()
        }
        Button {
            id: buttonPauseDownload
            enabled: false
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Pause download"
            onClicked: QtAndroidApkExpansionFiles.pauseDownload()
        }
        Button {
            id: buttonContinueDownload
            enabled: false
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Continue download"
            onClicked: QtAndroidApkExpansionFiles.continueDownload()
        }
        Button {
            id: buttonAbortDownload
            enabled: false
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Abort download"
            onClicked: QtAndroidApkExpansionFiles.abortDownload()
        }

        Label {
            id: downloadSizeLabel
            width: parent.width
            anchors.left: parent.left
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
            fontSizeMode: Text.Fit
        }
        Label {
            id: downloadTimeLabel
            width: parent.width
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
            fontSizeMode: Text.Fit
        }
    }

    MessageDialog {
        id: errorMessageBox
        buttons: MessageDialog.Ok
        title: "Error"
    }
    MessageDialog {
        id: fileDeliveredMessageBox
        buttons: MessageDialog.Ok
        title: "Advise"
        text: "Apk expension files available:\n" + QtAndroidApkExpansionFiles.mainFileName() + "\n" + QtAndroidApkExpansionFiles.patchFileName()
    }
}
