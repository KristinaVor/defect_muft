QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    myimageviewer.cpp \
    service_functions.cpp

HEADERS += \
    mainwindow.h \
    myimageviewer.h \
    service_functions.h \
    tiff.h \
    tiffconf.h \
    tiffio2.h \
    tiffvers.h

FORMS += \
    mainwindow.ui
    LIBS += -LC:\opencv\opencv\opencvBuild\install\x64\vc15\lib -lopencv_core410 -lopencv_imgproc410 -lopencv_highgui410 -lopencv_imgcodecs410 -lopencv_videoio410 -lopencv_video410 -lopencv_calib3d410 -lopencv_photo410 -lopencv_features2d410
    INCLUDEPATH += C:\opencv\opencv\opencvBuild\install\include
    DEPENDPATH += C:\opencv\opencv\opencvBuild\install\include


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
