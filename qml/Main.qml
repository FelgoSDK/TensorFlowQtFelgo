import Felgo 3.0
import QtQuick 2.0

App {
    // You get free licenseKeys from https://felgo.com/licenseKey
    // With a licenseKey you can:
    //  * Publish your games & apps for the app stores
    //  * Remove the Felgo Splash Screen or set a custom one (available with the Pro Licenses)
    //  * Add plugins to monetize, analyze & improve your apps (available with the Pro Licenses)
    //licenseKey: "<generate one from https://felgo.com/licenseKey>"

    id: app

    // Storage keys
    readonly property string kMinConfidence: "MinConfidence"
    readonly property string kModel:         "Model"
    readonly property string kShowTime:      "ShowTime"

    // Default values
    readonly property double defMinConfidence: 0.5
    readonly property string defModel: "ImageClassification"
    readonly property bool   defShowTime: false

    // Properties
    property double minConfidence
    property string model
    property bool   showTime

    // Local storage component
    Storage {
        id: storage

        Component.onCompleted: {
            minConfidence = getValue(kMinConfidence) !== undefined ? getValue(kMinConfidence) : defMinConfidence
            model         = getValue(kModel)         !== undefined ? getValue(kModel)         : defModel
            showTime      = getValue(kShowTime)      !== undefined ? getValue(kShowTime)      : defShowTime
        }
    }

    Navigation {

        NavigationItem{
            title: qsTr("Live")
            icon: IconType.rss

            NavigationStack{
                VideoPage{
                    id: videoPage
                    minConfidence: app.minConfidence
                    model: app.model
                    showTime: app.showTime
                }
            }
        }

        NavigationItem{
            title: qsTr("Settings")
            icon: IconType.sliders

            NavigationStack{
                AppSettingsPage{
                    id: appSettingsPage
                    minConfidence: app.minConfidence
                    model: app.model
                    showTime: app.showTime

                    onMinConfidenceChanged: {
                        app.minConfidence = appSettingsPage.minConfidence
                        storage.setValue(kMinConfidence,app.minConfidence)
                    }

                    onModelChanged: {
                        app.model = appSettingsPage.model
                        storage.setValue(kModel,app.model)
                    }

                    onShowTimeChanged: {
                        app.showTime = appSettingsPage.showTime
                        storage.setValue(kShowTime,app.showTime)
                    }
                }
            }
        }
    }
}
