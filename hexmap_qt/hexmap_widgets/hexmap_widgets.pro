#-------------------------------------------------
#
# Project created by QtCreator 2016-10-28T17:17:49
#
#-------------------------------------------------

QT       += core gui widgets opengl

CONFIG += c++14 console

QMAKE_CXXFLAGS += -fms-extensions -Wno-missing-braces -Wno-writable-strings
#QMAKE_CXXFLAGS_WARN_OFF += -Wno-missing-braces -Wno-writable-strings


TARGET = hexmap_widgets
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    hexmap_widget.cpp \
    layers_widget.cpp \
    tools_panel.cpp \
    pallet_widget.cpp

HEADERS  += mainwindow.h \
    hexmap_widget.h \
    layers_widget.h \
    tools_panel.h \
    pallet_widget.h

FORMS    += mainwindow.ui \
    tools_panel.ui \
    layers_widget.ui \
    pallet_widget.ui


# hexmap shaders
DISTFILES += \
    ../../shaders/330/hexmap_330.frag \
    ../../shaders/330/hexmap_330.vert



#asdf shared includes
INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../ext
INCLUDEPATH += $$PWD/../../include

INCLUDEPATH += $$PWD/../../hexmap/src


# libasdfm
unix:!macx: LIBS += -L$$PWD/../../lib/linux/ -lasdfm

INCLUDEPATH += $$PWD/../../asdf_multiplat
DEPENDPATH += $$PWD/../../asdf_multiplat
###

# libhexmap
unix:!macx: LIBS += -L$$PWD/../../lib/linux/ -lhexmap

INCLUDEPATH += $$PWD/../../hexmap/src
DEPENDPATH += $$PWD/../../hexmap/src
###

# apparently this is required
unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += glew ftgl sdl2
