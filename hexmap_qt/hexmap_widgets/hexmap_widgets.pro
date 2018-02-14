#-------------------------------------------------
#
# Project created by QtCreator 2016-10-28T17:17:49
#
#-------------------------------------------------

TARGET = hexmap_widgets
TEMPLATE = app

QT += core gui widgets opengl

CONFIG += release c++14 console

debug: DEFINES += DEBUG

SOURCES += main.cpp\
        mainwindow.cpp \
    hexmap_widget.cpp \
    layers_widget.cpp \
    tools_panel.cpp \
    palette_widget.cpp \
    dialogs/new_map_dialog.cpp \
    spline_settings_widget.cpp \
    object_properties_widget.cpp \
    minimap_widget.cpp \
    dialogs/map_properties_dialog.cpp \
    dialogs/export_image_dialog.cpp \
    terrain_brush_selector.cpp \
    snap_points_widget.cpp

HEADERS  += mainwindow.h \
    hexmap_widget.h \
    layers_widget.h \
    tools_panel.h \
    palette_widget.h \
    dialogs/new_map_dialog.h \
    spline_settings_widget.h \
    object_properties_widget.h \
    minimap_widget.h \
    dialogs/map_properties_dialog.h \
    dialogs/export_image_dialog.h \
    terrain_brush_selector.h \
    snap_points_widget.h

FORMS    += mainwindow.ui \
    layers_widget.ui \
    palette_widget.ui \
    dialogs/new_map_dialog.ui \
    spline_settings_widget.ui \
    object_properties_widget.ui \
    dialogs/map_properties_dialog.ui \
    dialogs/export_image_dialog.ui \
    terrain_brush_selector.ui \
    snap_points_widget.ui

# hexmap shaders
DISTFILES += \
    ../../shaders/330/hexmap_330.frag \
    ../../shaders/330/hexmap_330.vert



#asdf shared includes
INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../ext
INCLUDEPATH += $$PWD/../../include

#INCLUDEPATH += $$PWD/../../hexmap/src


# GCC / Clang Compiler Flags
!win32: QMAKE_CXXFLAGS += \
    -Werror=uninitialized \
    -Werror=return-type

!win32: QMAKE_CXXFLAGS_WARN_ON += \
    -Wno-missing-braces \
    -Wno-writable-strings \
    -Wno-write-strings \
    -Wno-format-zero-length \
    -Wno-expansion-to-defined \
    -Wno-reorder \
    -Wno-unused-parameter \
    -Wno-comment \




### Libraries ###
# Windows
win32: DEFINES += ASDFM_DLL
win32: LIBS += -L$$PWD/../../lib/win32/x64/ -lglu32 -lopengl32 -lglew64 -lSDL2 -lSDL2main -lSOIL_static_D -lzlibwapi
win32:debug:   LIBS += -lAsdfMultiplat_D
win32:release: LIBS += -lAsdfMultiplat
win32:debug:   LIBS += -lhexmap_D
win32:release: LIBS += -lhexmap
win32:CONFIG(release, debug|release):    LIBS += -L$$PWD/../../../Qt-Color-Widgets/build-color_widgets-Desktop_Qt_5_7_0_MSVC2015_64bit-Debug/release/ -lColorWidgets-qt51
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Qt-Color-Widgets/build-color_widgets-Desktop_Qt_5_7_0_MSVC2015_64bit-Debug/debug/   -lColorWidgets-qt51

# Linux
unix:!macx: LIBS += -L$$PWD/../../lib/linux/ -lasdfm -lhexmap
unix:!macx: LIBS += -L$$PWD/../../../Qt-Color-Widgets/build-color_widgets-Desktop-Debug/ -lColorWidgets-qt5
unix:!macx: CONFIG += link_pkgconfig
unix:!macx: PKGCONFIG += glew sdl2 zlib


# Lib includes/depends
# WIN32 *MUST* include asdfm and hexmap via the junction link in asdf_multiplat/include/
# otherwise it will break #pragma once, since the compiler will think the file is
# from a different location (because Windows is stupid and can't make proper links)
INCLUDEPATH += $$PWD/../../include/asdfm
INCLUDEPATH += $$PWD/../../include/hexmap
INCLUDEPATH += $$PWD/../../../Qt-Color-Widgets/include
#DEPENDPATH += $$PWD/../../asdf_multiplat/src
#DEPENDPATH += $$PWD/../../hexmap/src
#DEPENDPATH += $$PWD/../../../Qt-Color-Widgets/include
