import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

ScrollablePage {
    id: page
    padding: 20

    Column {
        anchors.fill: parent
        spacing: 5

        Repeater {
            model: QtAndroidImages.getAlbumsList();
            Label {
                font.bold: true
                font.pixelSize: 15
                text: model.name
            }
        }
    }
}
