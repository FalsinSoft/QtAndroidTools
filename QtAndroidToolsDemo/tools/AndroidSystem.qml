import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtAndroidTools

ScrollablePage {
    id: page
    padding: 20

    Column {
        anchors.fill: parent
        spacing: 5

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "DataLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.dataLocation
            width: parent.width
            wrapMode: Label.Wrap
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "ConfigLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.configLocation
            width: parent.width
            wrapMode: Label.Wrap
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "DownloadLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.downloadLocation
            width: parent.width
            wrapMode: Label.Wrap
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "MusicLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.musicLocation
            width: parent.width
            wrapMode: Label.Wrap
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "MoviesLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.moviesLocation
            width: parent.width
            wrapMode: Label.Wrap
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "PicturesLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.picturesLocation
            width: parent.width
            wrapMode: Label.Wrap
        }
    }
}
