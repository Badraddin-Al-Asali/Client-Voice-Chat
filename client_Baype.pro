QT += core websockets multimedia
QT += gui widgets
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    sslclient.cpp \
    cameraframegrabber.cpp \
    audioencoder.cpp

HEADERS += \
    sslclient.h \
    cameraframegrabber.h \
    audioencoder.h

INCLUDEPATH += F:\FFMPEG4_0_2\ffmpeg-4.0.2-win32-dev\include

LIBS += F:/FFMPEG4_0_2/ffmpeg-4.0.2-win32-dev/lib/libavdevice.dll.a
LIBS += F:/FFMPEG4_0_2/ffmpeg-4.0.2-win32-dev/lib/libavfilter.dll.a
LIBS += F:/FFMPEG4_0_2/ffmpeg-4.0.2-win32-dev/lib/libavformat.dll.a
LIBS += F:/FFMPEG4_0_2/ffmpeg-4.0.2-win32-dev/lib/libavutil.dll.a
LIBS += F:/FFMPEG4_0_2/ffmpeg-4.0.2-win32-dev/lib/libpostproc.dll.a
LIBS += F:/FFMPEG4_0_2/ffmpeg-4.0.2-win32-dev/lib/libswresample.dll.a
LIBS += F:/FFMPEG4_0_2/ffmpeg-4.0.2-win32-dev/lib/libswscale.dll.a
LIBS += F:/FFMPEG4_0_2/ffmpeg-4.0.2-win32-dev/lib/libavcodec.dll.a
