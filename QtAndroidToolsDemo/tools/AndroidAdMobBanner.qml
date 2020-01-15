
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    Column {
        width: parent.wdith
        height: parent.height * 0.9
        anchors.centerIn: parent
        spacing: 20

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 15
            text: "Banner"
        }
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            border.width: 1
            border.color: "black"
            width: banner1.width
            height: banner1.height

            QtAndroidAdMobBanner {
                id: banner1
                unitId: "ca-app-pub-3940256099942544/6300978111"
                type: QtAndroidAdMobBanner.TYPE_BANNER
                keywords: ["keyword_1", "keyword_2", "keyword_3"]
                onLoading: banner1state.text = "Loading"
                onLoaded: banner1state.text = "Loaded"
                onLoadError: banner1state.text = "Error " + errorId
            }
        }
        Label {
            id: banner1state
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13
            text: "Banner not loaded"
        }

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 15
            text: "Smart Banner"
        }
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            border.width: 1
            border.color: "black"
            width: banner2.width
            height: banner2.height

            QtAndroidAdMobBanner {
                id: banner2
                unitId: "ca-app-pub-3940256099942544/6300978111"
                type: QtAndroidAdMobBanner.TYPE_SMART_BANNER
                keywords: ["keyword_1", "keyword_2", "keyword_3"]
                onLoading: banner2state.text = "Loading"
                onLoaded: banner2state.text = "Loaded"
                onLoadError: banner2state.text = "Error " + errorId
            }
        }
        Label {
            id: banner2state
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13
            text: "Banner not loaded"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show banners"
            onClicked: {
                banner1.show();
                banner2.show();
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Hide banners"
            onClicked: {
                banner1.hide();
                banner2.hide();
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Reload banners"
            onClicked: {
                banner1.reload();
                banner2.reload();
            }
        }
    }
}
