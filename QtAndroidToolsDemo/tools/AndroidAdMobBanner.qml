
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 20

    Column {
        anchors.fill: parent
        spacing: 30

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "Banner 1"
        }
        QtAndroidAdMobBanner {
            id: banner1
            unitId: "ca-app-pub-3940256099942544/6300978111"
            type: QtAndroidAdMobBanner.TYPE_BANNER
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "Banner 2"
        }
        QtAndroidAdMobBanner {
            id: banner2
            unitId: "ca-app-pub-3940256099942544/6300978111"
            type: QtAndroidAdMobBanner.TYPE_BANNER
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Show banners"
            onClicked: {
                banner1.show();
                banner2.show();
            }
        }
    }
}
