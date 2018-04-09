import QtQuick 2.10
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import QtMultimedia 5.7

Page {
    id: page
    bottomPadding: 0
    topPadding: 0

    Item {
        id: previewItem
        anchors.fill: parent

        Label {
            text: "No Basler camera detected."
            anchors.centerIn: parent
            visible: !pylonCamera.isOpen
        }

        VideoOutput {
            id: viewFinder
            anchors.fill: parent
            visible: pylonCamera.isOpen

            source: pylonCamera
            fillMode: VideoOutput.PreserveAspectCrop
            autoOrientation: true

            Label {
                text: pylonCamera.name
                color: "white"
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    bottomMargin: 6
                    leftMargin: 6
                }
            }

        }
            Button {
                text: "Capture"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom: parent.bottom
                    bottomMargin: 16
                }
                onClicked: {
                    pylonCamera.capture();
                }
            }

        Connections {
            target: pylonCamera
            onImageCaptured: {
                console.log('Captured: ' + path);
                previewImage.source = 'file://' + path;
            }
        }

        Image {
            id: previewImage
            anchors.fill: parent
            fillMode: VideoOutput.PreserveAspectCrop
            visible: source != null
        }
    }
}
