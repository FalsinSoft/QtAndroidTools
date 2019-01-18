import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools.ApkInfo 1.0

Page {
    id: page
    padding: 20

    Column {
        anchors.fill: parent
        spacing: 5

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "FirstInstallTime:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidApkInfo.firstInstallTime
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "LastUpdateTime:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidApkInfo.lastUpdateTime
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "PackageName:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidApkInfo.packageName
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "VersionCode:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidApkInfo.versionCode
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "VersionName:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidApkInfo.versionName
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "RequestedPermissions:"
        }
        Column {
            width: parent.width
            Repeater {
                model: QtAndroidApkInfo.requestedPermissions
                Label {
                    font.pixelSize: 15
                    text: modelData
                }
            }
        }
    }
}
