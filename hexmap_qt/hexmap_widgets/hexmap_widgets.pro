#-------------------------------------------------
#
# Project created by QtCreator 2016-10-28T17:17:49
#
#-------------------------------------------------

QT       += core gui widgets opengl

CONFIG += c++14 console

!win32: QMAKE_CXXFLAGS += -fms-extensions -Wno-missing-braces -Wno-writable-strings
#QMAKE_CXXFLAGS_WARN_OFF += -Wno-missing-braces -Wno-writable-strings


debug: DEFINES += DEBUG

#include( ../../../Qt-Color-Widgets/color_widgets.pri )


TARGET = hexmap_widgets
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    hexmap_widget.cpp \
    layers_widget.cpp \
    tools_panel.cpp \
    palette_widget.cpp \
    dialogs/new_map_dialog.cpp \
    spline_settings_widget.cpp \
    object_properties_widget.cpp

HEADERS  += mainwindow.h \
    hexmap_widget.h \
    layers_widget.h \
    tools_panel.h \
    palette_widget.h \
    dialogs/new_map_dialog.h \
    spline_settings_widget.h \
    object_properties_widget.h

FORMS    += mainwindow.ui \
    tools_panel.ui \
    layers_widget.ui \
    palette_widget.ui \
    dialogs/new_map_dialog.ui \
    spline_settings_widget.ui \
    object_properties_widget.ui


# hexmap shaders
DISTFILES += \
    ../../shaders/330/hexmap_330.frag \
    ../../shaders/330/hexmap_330.vert



#asdf shared includes
INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../ext
INCLUDEPATH += $$PWD/../../include

#INCLUDEPATH += $$PWD/../../hexmap/src


# libasdfm
unix:!macx: LIBS += -L$$PWD/../../lib/linux/ -lasdfm
win32: LIBS += -L$$PWD/../../lib/win32/x64/ -lAsdfMultiplat_static_D

INCLUDEPATH += $$PWD/../../asdf_multiplat
DEPENDPATH += $$PWD/../../asdf_multiplat

#TODO: handle Debug vs Release
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../lib/win32/x64/AsdfMultiplat_static_D.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../lib/win32/x64/libAsdfMultiplat_static_D.a
###


# libhexmap
unix:!macx: LIBS += -L$$PWD/../../lib/linux/ -lhexmap
win32: LIBS += -L$$PWD/../../lib/win32/x64/ -lhexmap_static_D

# WIN32 *MUST* include hexmap via the junction link in asdf_multiplat/include/
# otherwise it will break #pragma once, since the compiler will think the file is
# from a different location (because Windows is stupid and can't make proper links)
INCLUDEPATH += $$PWD/../../include/hexmap
#INCLUDEPATH += $$PWD/../../hexmap/src
DEPENDPATH += $$PWD/../../hexmap/src

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../lib/win32/x64/hexmap_static_D.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../lib/win32/x64/libhexmap_static_D.a
###


# apparently this is required
unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += glew ftgl sdl2




# Qt-Color-Widgets
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Qt-Color-Widgets/build-color_widgets-Desktop_Qt_5_7_0_MSVC2015_64bit-Debug/release/ -lColorWidgets-qt51
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Qt-Color-Widgets/build-color_widgets-Desktop_Qt_5_7_0_MSVC2015_64bit-Debug/debug/ -lColorWidgets-qt51

unix:!macx: LIBS += -L$$PWD/../../../build-color_widgets-Desktop_Qt_5_7_0_Clang_64bit-Debug/ -lColorWidgets-qt5

INCLUDEPATH += $$PWD/../../../Qt-Color-Widgets/include
DEPENDPATH += $$PWD/../../../Qt-Color-Widgets/include
