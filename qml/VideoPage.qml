import QtQuick 2.0
import QtMultimedia 5.9
import Felgo 3.0

import ObjectsRecognizer 1.0

Page {
    id: root
    title: qsTr("Live")

    // Properties
    property double minConfidence
    property string model
    property bool showTime

    // Selected camera index
    property int cameraIndex: 0

    // Start and stop camera
    onVisibleChanged: {
        if (visible) camera.start()
        else camera.stop()
    }

    // Right-hand side buttons
    rightBarItem: NavigationBarRow {

        // Switch camera button
        IconButtonBarItem {
            title: qsTr("Switch camera")
            visible: camera.availableCameras
            icon: IconType.videocamera
            onClicked: {
                console.log("Camera index: " + cameraIndex)
                cameraIndex = (cameraIndex+1) % QtMultimedia.availableCameras.length
                camera.deviceId = QtMultimedia.availableCameras[cameraIndex].deviceId
                videoOutput.rotation = initialRotation()
            }
        }
    }

    Camera{
        id: camera
        property bool availableCamera:  QtMultimedia.availableCameras.length>0
        property bool availableCameras: QtMultimedia.availableCameras.length>1
    }

    VideoOutput {
        id: videoOutput
        anchors.fill: parent
        source: camera
        visible: camera.availableCamera && camera.cameraStatus == Camera.ActiveStatus
        autoOrientation: true
        fillMode: VideoOutput.PreserveAspectCrop
        rotation: initialRotation()

        filters: [objectsRecognitionFilter]
    }

    ObjectsRecognizer {
        id: objectsRecognitionFilter
        cameraOrientation: camera.orientation
        videoOrientation: videoOutput.orientation - initialRotation()
        contentSize: Qt.size(videoOutput.width,videoOutput.height)

        minConfidence: root.minConfidence
        model:         root.model
        showTime:      root.showTime
    }

    // No camera found
    Item{
        anchors.centerIn: parent
        width: parent.width
        visible: !camera.availableCamera
        Column{
            width: parent.width
            spacing: dp(25)

            Icon{
                anchors.horizontalCenter: parent.horizontalCenter
                icon: IconType.videocamera
                scale: 3
            }

            AppText{
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("No camera detected")
            }
        }
    }

    // Loading camera
    Item{
        anchors.centerIn: parent
        width: parent.width
        visible: camera.availableCamera && camera.cameraStatus != Camera.ActiveStatus
        Column{
            width: parent.width
            spacing: dp(25)

            Icon{
                id: videoIcon
                anchors.horizontalCenter: parent.horizontalCenter
                icon: IconType.videocamera
                scale: 3

                SequentialAnimation {
                       running: true
                       loops: Animation.Infinite
                       NumberAnimation { target: videoIcon; property: "opacity"; from: 1; to: 0; duration: 500 }
                       NumberAnimation { target: videoIcon; property: "opacity"; from: 0; to: 1; duration: 500 }
                 }
            }

            AppText{
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Loading camera") + " ..."
            }
        }
    }

    // BUG: front camera rotation on ios [QTBUG-37955]
    // Qt.platform.os === "ios", system.isIos, Theme.isIos
    function initialRotation()
    {
        return Qt.platform.os === "ios" && camera.position === Camera.FrontFace ? 180 : 0
    }
}
