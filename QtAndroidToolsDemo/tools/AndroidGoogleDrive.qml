
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

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
            height: parent.height * 0.4
            border.width: 1
            border.color: "black"
            clip: true

            ListModel { id: filesListModel }

            ListView {
                id: filesListView
                width: parent.width * 0.95
                height: parent.height
                anchors.centerIn: parent
                model: filesListModel
                boundsBehavior: Flickable.StopAtBounds
                delegate: Item {
                    width: filesListView.width
                    height: fileInfoColumn.implicitHeight + 10
                    Column {
                        id: fileInfoColumn
                        Text { font.pixelSize: 13; text: '<b>Id:</b> ' + id }
                        Text { font.pixelSize: 13; text: '<b>Name:</b> ' + name }
                        Text { font.pixelSize: 13; text: '<b>MimeType:</b> ' + mimeType }
                        Text { font.pixelSize: 13; text: '<b>ParentId:</b> ' + parentId }
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
}
