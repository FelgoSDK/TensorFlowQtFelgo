import Felgo 3.0
import QtQuick 2.0
import QtQuick.Controls 1.4

Page {
    title: qsTr("Settings")
    id: root

    // Properties
    property double minConfidence
    property string model
    property bool   showTime

    Column{
        id:    column
        width: root.width

        Item{ height: dp(20); width: 1 }

        AppText{
            anchors.margins: dp(20)
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: AppText.AlignHCenter
            width: parent.width
            wrapMode: AppText.WordWrap
            elide: AppText.ElideRight
            text: qsTr("Minimum confidence")
        }

        Item{ height: dp(5); width: 1 }

        AppSlider{
            id: slider
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 2*dp(20)
            from:  0
            to:    1
            value: minConfidence
            live:  true
            onValueChanged: minConfidence = value
        }

        AppText {
            anchors.margins: dp(20)
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: AppText.AlignHCenter
            width: parent.width
            wrapMode: AppText.WordWrap
            elide: AppText.ElideRight
            color: Theme.tintColor
            text: Math.round(slider.position * 100) + " %"
        }

        Item{ height: dp(10); width: 1 }

        Row{
            width: parent.width - 2*dp(20)
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: width - tShowInfTime.width - sShowInfTime.width

            AppText {
                id: tShowInfTime
                text: qsTr("Show inference time")
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: AppText.AlignVCenter
            }

            AppSwitch{
                anchors.verticalCenter: parent.verticalCenter
                id: sShowInfTime
                checked: showTime
                onToggled: showTime = checked
            }
        }

        Item{ height: dp(20); width: 1 }

        AppText{
            anchors.margins: dp(20)
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: AppText.AlignHCenter
            width: parent.width
            wrapMode: AppText.WordWrap
            elide: AppText.ElideRight
            text: qsTr("Tensorflow model")
        }

        Item{ height: dp(20); width: 1 }

        ExclusiveGroup { id: modelGroup }

        AppCheckBox{
            id: chkClassification
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 2*dp(20)
            text: qsTr("Image classification")
            exclusiveGroup: modelGroup
            checked: model === "ImageClassification"
            onCheckedChanged: if (checked) model = "ImageClassification"; else chkDetection.checked = true
        }

        Item{ height: dp(20); width: 1 }

        AppCheckBox{
            id: chkDetection
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 2*dp(20)
            text: qsTr("Object detection")
            exclusiveGroup: modelGroup
            checked: model === "ObjectDetection"
            onCheckedChanged: if (checked) model = "ObjectDetection"; else chkClassification.checked = true
        }
    }
}
