import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import QtAndroidTools 1.0

Page {
    id: page
    padding: 20

    readonly property var permissionsNameList: ["android.permission.WRITE_EXTERNAL_STORAGE","android.permission.READ_CALENDAR","android.permission.READ_PHONE_STATE","android.permission.READ_CONTACTS"]

    function setPermissionGranted(name, granted)
    {
        for(var i = 0; i < appPermissionModel.count; i++)
        {
            if(appPermissionModel.get(i).name === name)
            {
                appPermissionModel.get(i).symbol = granted ? "qrc:/images/correct.svg" : "qrc:/images/error.svg";
                break;
            }
        }
    }

    Connections {
        target: QtAndroidAppPermissions
        function onRequestPermissionsResults(results)
        {
            for(var i = 0; i < results.length; i++)
            {
                if(results[i].granted === true)
                {
                    setPermissionGranted(results[i].name, true);
                }
                else
                {
                    if(QtAndroidAppPermissions.shouldShowRequestPermissionInfo(results[i].name) === true)
                    {
                        if(results[i].name === permissionsNameList[0])
                            requestPermissionWRITE_EXTERNAL_STORAGE.open();
                        else if(results[i].name === permissionsNameList[1])
                            requestPermissionREAD_CALENDAR.open();
                        else if(results[i].name === permissionsNameList[2])
                            requestPermissionREAD_PHONE_STATE.open();
                        else if(results[i].name === permissionsNameList[3])
                            requestPermissionREAD_CONTACTS.open();
                    }
                    else
                    {
                        setPermissionGranted(results[i].name, false);
                    }
                }
            }
        }
    }

    ListModel {
        id: appPermissionModel
        ListElement {
            name: "android.permission.WRITE_EXTERNAL_STORAGE"
            symbol: "qrc:/images/unknown.svg"
        }
        ListElement {
            name: "android.permission.READ_CALENDAR"
            symbol: "qrc:/images/unknown.svg"
        }
        ListElement {
            name: "android.permission.READ_PHONE_STATE"
            symbol: "qrc:/images/unknown.svg"
        }
        ListElement {
            name: "android.permission.READ_CONTACTS"
            symbol: "qrc:/images/unknown.svg"
        }
    }

    Column {
        anchors.fill: parent
        spacing: 20

        Repeater {
            model: appPermissionModel
            Rectangle {
                width: parent.width
                height: premissionStatus.height + 20
                color: "gainsboro"
                radius: 15

                Item {
                    width: parent.width * 0.9
                    height: premissionStatus.height
                    anchors.centerIn: parent

                    Image {
                        id: premissionStatus
                        anchors.left: parent.left
                        fillMode: Image.PreserveAspectFit
                        source: symbol
                        sourceSize.height: permissionName.contentHeight * 1.5
                    }
                    Text {
                        id: permissionName
                        width: parent.width - premissionStatus.width - 10
                        anchors.right: parent.right
                        text: name
                        font.pixelSize: parent.width * 0.04
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Request permissions"
            onClicked: QtAndroidAppPermissions.requestPermissions(permissionsNameList)
        }
    }

    MessageDialog {
        id: requestPermissionWRITE_EXTERNAL_STORAGE
        buttons: MessageDialog.Ok
        title: "Advise"
        text: "This app require WRITE_EXTERNAL_STORAGE permission for bla bla bla..."
        onAccepted: QtAndroidAppPermissions.requestPermission(permissionsNameList[0])
    }
    MessageDialog {
        id: requestPermissionREAD_CALENDAR
        buttons: MessageDialog.Ok
        title: "Advise"
        text: "This app require READ_CALENDAR permission for bla bla bla..."
        onAccepted: QtAndroidAppPermissions.requestPermission(permissionsNameList[1])
    }
    MessageDialog {
        id: requestPermissionREAD_PHONE_STATE
        buttons: MessageDialog.Ok
        title: "Advise"
        text: "This app require READ_PHONE_STATE permission for bla bla bla..."
        onAccepted: QtAndroidAppPermissions.requestPermission(permissionsNameList[2])
    }
    MessageDialog {
        id: requestPermissionREAD_CONTACTS
        buttons: MessageDialog.Ok
        title: "Advise"
        text: "This app require READ_CONTACTS permission for bla bla bla..."
        onAccepted: QtAndroidAppPermissions.requestPermission(permissionsNameList[3])
    }
}
