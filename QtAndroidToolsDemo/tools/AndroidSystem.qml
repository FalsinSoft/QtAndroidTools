import QtQuick
import QtQuick.Controls
import QtAndroidTools 1.0

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
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "ConfigLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.configLocation
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "DownloadLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.downloadLocation
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "MusicLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.musicLocation
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "MoviesLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.moviesLocation
        }

        Label {
            font.bold: true
            font.pixelSize: 15
            text: "PicturesLocation:"
        }
        Label {
            font.pixelSize: 15
            text: QtAndroidSystem.picturesLocation
        }
    }
}
