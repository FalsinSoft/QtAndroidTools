import QtQuick
import QtQuick.Controls.Material
import QtAndroidTools

Page {
    id: page
    padding: 0

    property var albumsNameList: []

    function showAlbumsImages(index)
    {
        if(index < albumsNameList.length)
        {
            imagesGrid.model = QtAndroidImages.getAlbumImagesList(albumsNameList[index]);
        }
    }

    Component.onCompleted: {
        albumsNameList = QtAndroidImages.getAlbumsList();
        showAlbumsImages(0);
    }

    Rectangle {
        id: topBar
        anchors.top: parent.top
        width: parent.width
        height: albumsRow.implicitHeight * 1.5
        color: "white"

        Row {
            id: albumsRow
            anchors.centerIn: parent
            spacing: 10

            Text {
                height: parent.height
                text: "Albums"
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 20
                rightPadding: 20
            }

            ComboBox {
                id: albums
                model: page.albumsNameList
                onActivated: showAlbumsImages(index)
            }
        }
    }

    GridView {
        id: imagesGrid
        width: parent.width
        height: parent.height - topBar.height
        anchors.bottom: parent.bottom
        boundsBehavior: Flickable.StopAtBounds
        cellWidth: width / 3
        cellHeight: cellWidth
        delegate: Image {
            width: imagesGrid.cellWidth
            height: imagesGrid.cellHeight
            source: "file://" + modelData
            sourceSize.width: width
            sourceSize.height: height
            asynchronous: true
            cache: false
        }
    }
}
