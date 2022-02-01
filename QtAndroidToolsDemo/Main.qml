
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import QtAndroidTools

ApplicationWindow {
    id: window
    visible: true
    width: 360
    height: 520
    title: "QtAndroidTools Demo"
    Component.onCompleted: {
        console.log(QtAndroidBatteryState.onCharge)
    }

//    Component.onCompleted: {
//        if(QtAndroidTools.activityAction !== QtAndroidTools.ACTION_NONE)
//        {
//            stackView.push("qrc:/tools/AndroidSharing.qml");
//        }
//    }

    header: ToolBar {
        Material.foreground: "white"

        RowLayout {
            spacing: 20
            anchors.fill: parent

            ToolButton {
                icon.name: stackView.depth > 1 ? "back" : "drawer"
                onClicked: {
                    if (stackView.depth > 1) {
                        stackView.pop()
                        listView.currentIndex = -1
                    } else {
                        drawer.open()
                    }
                }
            }

            Label {
                id: titleLabel
                text: listView.currentItem ? listView.currentItem.text : "Android Tools"
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                icon.name: "menu"
                onClicked: optionsMenu.open()

                Menu {
                    id: optionsMenu
                    x: parent.width - width
                    transformOrigin: Menu.TopRight

                    MenuItem {
                        text: "About"
                        onTriggered: aboutDialog.open()
                    }
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(window.width, window.height) / 3 * 2
        height: window.height
        interactive: stackView.depth === 1

        ListView {
            id: listView

            focus: true
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate {
                width: parent.width
                text: model.title
                highlighted: ListView.isCurrentItem
                onClicked: {
                    listView.currentIndex = index
                    stackView.push(model.source)
                    drawer.close()
                }
            }

            model: ListModel {
//                ListElement { title: "AppPermissions"; source: "qrc:/tools/AndroidAppPermissions.qml" }
//                ListElement { title: "ApkExpansionFiles"; source: "qrc:/tools/AndroidApkExpansionFiles.qml" }
                ListElement { title: "ApkInfo"; source: "qrc:/tools/AndroidApkInfo.qml" }
                ListElement { title: "Screen"; source: "qrc:/tools/AndroidScreen.qml" }
                ListElement { title: "BatteryState"; source: "qrc:/tools/AndroidBatteryState.qml" }
//                ListElement { title: "SignalStrength"; source: "qrc:/tools/AndroidSignalStrength.qml" }
//                ListElement { title: "Images"; source: "qrc:/tools/AndroidImages.qml" }
                ListElement { title: "Notification"; source: "qrc:/tools/AndroidNotification.qml" }
//                ListElement { title: "AdMobBanner"; source: "qrc:/tools/AndroidAdMobBanner.qml" }
//                ListElement { title: "AdMobInterstitial"; source: "qrc:/tools/AndroidAdMobInterstitial.qml" }
//                ListElement { title: "AdMobRewardedVideo"; source: "qrc:/tools/AndroidAdMobRewardedVideo.qml" }
//                ListElement { title: "PlayStore"; source: "qrc:/tools/AndroidPlayStore.qml" }
//                ListElement { title: "GoogleAccount"; source: "qrc:/tools/AndroidGoogleAccount.qml" }
//                ListElement { title: "GoogleDrive"; source: "qrc:/tools/AndroidGoogleDrive.qml" }
                ListElement { title: "Sharing"; source: "qrc:/tools/AndroidSharing.qml" }
//                ListElement { title: "UserMessagingPlatform"; source: "qrc:/tools/AndroidUserMessagingPlatform.qml" }
//                ListElement { title: "Audio"; source: "qrc:/tools/AndroidAudio.qml" }
                ListElement { title: "System"; source: "qrc:/tools/AndroidSystem.qml" }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: Pane {
            id: pane

            Image {
                id: logo
                width: pane.availableWidth / 3
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -50
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/logo_falsinsoft.jpg"
            }

            Label {
                text: "Small collections of tools to manage some Android features from Qt and QML app"
                anchors.margins: 20
                anchors.top: logo.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter
                wrapMode: Label.Wrap
            }
        }
    }

    Dialog {
        id: aboutDialog
        modal: true
        focus: true
        title: "About"
        x: (window.width - width) / 2
        y: window.height / 6
        width: Math.min(window.width, window.height) / 3 * 2
        contentHeight: copyrightLabel.height

        Label {
            id: copyrightLabel
            width: aboutDialog.availableWidth
            text: "Copyright (c) 2018 Fabio Falsini\n\n"
                + "https://falsinsoft.blogspot.com"
            wrapMode: Label.Wrap
            font.pixelSize: 12
        }
    }

}
