import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools
import Qt.labs.platform

Page {
    id: page
    padding: 0

    Connections {
        target: QtAndroidGoogleDrive
        function onDownloadProgressChanged(state, progress)
        {
            switch(state)
            {
                case QtAndroidGoogleDrive.STATE_MEDIA_IN_PROGRESS:
                    break;
                case QtAndroidGoogleDrive.STATE_MEDIA_COMPLETE:
                    downloadCompleteMsg.open();
                    break;
            }
        }
        function onUploadProgressChanged(state, progress)
        {
            switch(state)
            {
                case QtAndroidGoogleDrive.STATE_INITIATION_STARTED:
                    break;
                case QtAndroidGoogleDrive.STATE_INITIATION_COMPLETE:
                    break;
                case QtAndroidGoogleDrive.STATE_MEDIA_IN_PROGRESS:
                    break;
                case QtAndroidGoogleDrive.STATE_MEDIA_COMPLETE:
                    break;
            }
        }
    }

    Column {
        width: parent.width * 0.9
        height: parent.height * 0.9
        anchors.centerIn: parent
        spacing: 5

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Authenticate"
            onClicked: QtAndroidGoogleDrive.authenticate("QtAndroidTools", QtAndroidGoogleDrive.SCOPE_DRIVE)
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: QtAndroidGoogleDrive.isAuthenticated
            text: "Get files list"
            onClicked: {
                var filesList = QtAndroidGoogleDrive.getFilesList();
                var rootId = QtAndroidGoogleDrive.getRootId();

                filesListModel.clear();
                for(var i = 0; i < filesList.length; i++)
                {
                    var data = filesList[i];
                    var parentId = "null";

                    if(data.parents.length > 0)
                    {
                        if(data.parents[0] === rootId)
                            parentId = "root";
                        else
                            parentId = data.parents[0];
                    }

                    filesListModel.append({ "id": data.id,
                                            "name": data.name,
                                            "mimeType": data.mimeType,
                                            "parentId": parentId
                                          });
                }
            }
        } 
        Rectangle {
            width: parent.width
            height: parent.height * 0.6
            border.width: 1
            border.color: "black"
            clip: true

            ListModel { id: filesListModel }

            ListView {
                id: filesListView
                width: parent.width * 0.95
                height: parent.height - 2
                anchors.centerIn: parent
                model: filesListModel
                boundsBehavior: Flickable.StopAtBounds
                delegate: Item {
                    width: filesListView.width
                    height: fileInfoColumn.implicitHeight + 10
                    Column {
                        id: fileInfoColumn
                        width: parent.width

                        Text { font.pixelSize: 13; text: '<b>Id:</b> ' + id }
                        Text { font.pixelSize: 13; text: '<b>Name:</b> ' + name }
                        Text { font.pixelSize: 13; text: '<b>MimeType:</b> ' + mimeType }
                        Text { font.pixelSize: 13; text: '<b>ParentId:</b> ' + parentId }

                        Button {
                            width: parent.width
                            text: "Download"
                            visible: !QtAndroidGoogleDrive.isFolder(id)
                            onClicked: {
                                if(QtAndroidAppPermissions.isPermissionGranted("android.permission.WRITE_EXTERNAL_STORAGE"))
                                {
                                    var downloadPath = QtAndroidSystem.getFolderPath(QtAndroidSystem.FOLDER_DOWNLOAD);

                                    if(QtAndroidGoogleDrive.downloadFile(id, downloadPath + "/" + name) === false)
                                    {
                                        downloadNotStartedMsg.open();
                                    }
                                }
                                else
                                {
                                    permissionNotGrantedMsg.open();
                                }
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    width: 10
                    anchors.left: parent.right
                    policy: ScrollBar.AlwaysOn
                }
            }
        }
    }

    MessageDialog {
        id: permissionNotGrantedMsg
        buttons: StandardButton.Ok
        title: "Warning"
        text: "For file download the app need the WRITE_EXTERNAL_STORAGE permission. Go in the App Permission section and request the permission"
    }
    MessageDialog {
        id: downloadCompleteMsg
        buttons: StandardButton.Ok
        title: "Advise"
        text: "Download is complete!"
    }
    MessageDialog {
        id: downloadNotStartedMsg
        buttons: StandardButton.Ok
        title: "Warning"
        text: "Download not started for an error!"
    }
}
