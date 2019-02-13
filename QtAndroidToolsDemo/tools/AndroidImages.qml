import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3
import QtAndroidTools 1.0

Page {
    id: page
    padding: 0

    property var albumsList: null

    function showAlbumsImages(index)
    {
        if(albumsList !== null && index < albumsList.length)
        {
            imagesGrid.model = QtAndroidImages.getAlbumImagesList(albumsList[index].id);
        }
    }

    Component.onCompleted: {
        var albumsNameList = new Array();

        albumsList = QtAndroidImages.getAlbumsList();

        for(var i = 0; i < albumsList.length; i++) albumsNameList.push(albumsList[i].name);
        albums.model = albumsNameList;
        albums.currentIndex = 0;

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
