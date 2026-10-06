import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

ScrollablePage {
    id: page
    padding: 0

    Column {
        width: parent.width
        height: parent.height * 0.9
        anchors.centerIn: parent
        spacing: 10

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
                trackMovement: true
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
            text: "Inline Adaptive Banner"
        }
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            border.width: 1
            border.color: "black"
            width: banner2.width
            height: banner2.height

            QtAndroidAdMobBanner {
                id: banner2
                trackMovement: true
                inlineAdaptiveBannerMaxHeight: 100
                unitId: "ca-app-pub-3940256099942544/6300978111"
                type: QtAndroidAdMobBanner.TYPE_INLINE_ADAPTIVE_BANNER
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

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 15
            text: "Large Anchored Adaptive Banner"
        }
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            border.width: 1
            border.color: "black"
            width: banner3.width
            height: banner3.height

            QtAndroidAdMobBanner {
                id: banner3
                trackMovement: true
                unitId: "ca-app-pub-3940256099942544/6300978111"
                type: QtAndroidAdMobBanner.TYPE_LARGE_ANCHORED_ADAPTIVE_BANNER
                keywords: ["keyword_1", "keyword_2", "keyword_3"]
                onLoading: banner3state.text = "Loading"
                onLoaded: banner3state.text = "Loaded"
                onLoadError: banner3state.text = "Error " + errorId
            }
        }
        Label {
            id: banner3state
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
                banner3.show();
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Hide banners"
            onClicked: {
                banner1.hide();
                banner2.hide();
                banner3.hide();
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Reload banners"
            onClicked: {
                banner1.reload();
                banner2.reload();
                banner3.reload();
            }
        }
    }
}
