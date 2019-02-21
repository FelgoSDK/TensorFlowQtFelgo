# allows to add DEPLOYMENTFOLDERS and links to the Felgo library and QtCreator auto-completion
CONFIG += felgo

# uncomment this line to add the Live Client Module and use live reloading with your custom C++ code
# for the remaining steps to build a custom Live Code Reload app see here: https://felgo.com/custom-code-reload-app/
#CONFIG += felgo-live

# configure the bundle identifier for iOS
PRODUCT_IDENTIFIER = com.yourcompany.wizardEVAP.TensorFlowQtFelgo

qmlFolder.source = qml

assetsFolder.source = assets
DEPLOYMENTFOLDERS += assetsFolder

# NOTE: for PUBLISHING, perform the following steps:
# 1. comment the DEPLOYMENTFOLDERS += qmlFolder line above, to avoid shipping your qml files with the application (instead they get compiled to the app binary)
# 2. uncomment the resources.qrc file inclusion and add any qml subfolders to the .qrc file; this compiles your qml files and js files to the app binary and protects your source code
# 3. change the setMainQmlFile() call in main.cpp to the one starting with "qrc:/" - this loads the qml files from the resources
# for more details see the "Deployment Guides" in the Felgo Documentation

# during development, use the qmlFolder deployment because you then get shorter compilation times (the qml files do not need to be compiled to the binary but are just copied)
# also, for quickest deployment on Desktop disable the "Shadow Build" option in Projects/Builds - you can then select "Run Without Deployment" from the Build menu in Qt Creator if you only changed QML files; this speeds up application start, because your app is not copied & re-compiled but just re-interpreted

# Debug or release mode

# This works for Linux and macOS, appropiate copy command must be set for Windows
CONFIG(debug,debug|release):system($$quote(cp "$$PWD/qml/config_debug.json" "$$PWD/qml/config.json"))
CONFIG(release,debug|release):system($$quote(cp "$$PWD/qml/config_release.json" "$$PWD/qml/config.json"))

CONFIG(debug,debug|release):DEPLOYMENTFOLDERS += qmlFolder # comment for publishing
CONFIG(release,debug|release):RESOURCES += resources.qrc # uncomment for publishing

CONFIG(release,debug|release):CONFIG += qtquickcompiler # Qt Quick compiler

QT += multimedia multimedia-private

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    tensorflow.cpp \
    objectsrecogfilter.cpp \
    tensorflowthread.cpp \
    auxutils.cpp \
    colormanager.cpp

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    OTHER_FILES += android/AndroidManifest.xml \
                   android/build.gradle
}

ios {
    QMAKE_INFO_PLIST = ios/Project-Info.plist
    OTHER_FILES += $$QMAKE_INFO_PLIST
}

# set application icons for win and macx
win32 {
    RC_FILE += win/app_icon.rc
}
macx {
    ICON = macx/app_icon.icns
}

# TensorFlow - All
TF_MAKE_PATH = $$PWD/tensorflow/tensorflow/contrib/makefile
INCLUDEPATH += $$PWD/tensorflow/ \
               $$TF_MAKE_PATH/gen/host_obj \
               $$TF_MAKE_PATH/downloads/eigen

# TensorFlow - Linux
linux:!android {
      INCLUDEPATH += $$TF_MAKE_PATH/gen/protobuf/include
      LIBS += -L$$TF_MAKE_PATH/downloads/nsync/builds/default.linux.c++11/ \
              -L$$TF_MAKE_PATH/gen/protobuf/lib64/ \
              -L$$TF_MAKE_PATH/gen/lib/ \
              -lnsync \
              -lprotobuf \
              -ltensorflow-core \
              -ldl
      QMAKE_LFLAGS += -Wl,--allow-multiple-definition -Wl,--whole-archive
}

# TensorFlow - Android
android {
    QT += androidextras
    LIBS += -ltensorflow-core -lprotobuf -lnsync -lcpufeatures \
            -L${ANDROID_NDK_ROOT}/sources/android/cpufeatures/obj/local/$$ANDROID_TARGET_ARCH
    QMAKE_LFLAGS += -Wl,--allow-multiple-definition -Wl,--whole-archive

    # Platform: armv7a
    equals(ANDROID_TARGET_ARCH, armeabi-v7a) | equals(ANDROID_TARGET_ARCH, armeabi):\
    {
        INCLUDEPATH += $$TF_MAKE_PATH/gen/protobuf_android/armeabi-v7a/include
        LIBS += -L$$TF_MAKE_PATH/gen/lib/android_armeabi-v7a \
                -L$$TF_MAKE_PATH/gen/protobuf_android/armeabi-v7a/lib \
                -L$$TF_MAKE_PATH/downloads/nsync/builds/armeabi-v7a.android.c++11
    }
    # Platform: x86
    equals(ANDROID_TARGET_ARCH, x86):\
    {
        INCLUDEPATH += $$TF_MAKE_PATH/gen/protobuf_android/x86/include
        LIBS += -L$$TF_MAKE_PATH/gen/lib/android_x86 \
                -L$$TF_MAKE_PATH/gen/protobuf_android/x86/lib \
                -L$$TF_MAKE_PATH/downloads/nsync/builds/x86.android.c++11
    }
}

# TensorFlow - iOS - Universal libraries
ios {
    INCLUDEPATH += $$TF_MAKE_PATH/gen/protobuf-host/include
    LIBS += -L$$PWD/ios/lib \
            -L$$PWD/ios/lib/arm64 \
            -framework Accelerate \
            -Wl,-force_load,$$TF_MAKE_PATH/gen/lib/libtensorflow-core.a \
            -Wl,-force_load,$$TF_MAKE_PATH/gen/protobuf_ios/lib/libprotobuf.a \
            -Wl,-force_load,$$TF_MAKE_PATH/downloads/nsync/builds/arm64.ios.c++11/libnsync.a
}

DISTFILES += \
    qml/VideoPage.qml \
    AppSettingsPage.qml

HEADERS += \
    tensorflow.h \
    objectsrecogfilter.h \
    tensorflowthread.h \
    auxutils.h \
    colormanager.h \
    get_top_n_impl.h \
    get_top_n.h
