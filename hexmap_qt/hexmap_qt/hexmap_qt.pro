TEMPLATE = app

QT += qml quick widgets

CONFIG += c++14

SOURCES += main.cpp \
    hexmap_framebuffer.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    hexmap_framebuffer.h \
    asdf_qt_renderer.h


INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../ext


unix:!macx: LIBS += -L$$PWD/../../lib/linux/ -lhexmap

INCLUDEPATH += $$PWD/../../include
DEPENDPATH += $$PWD/../../include

unix:!macx: LIBS += -L$$PWD/../../lib/linux/ -lasdfm

INCLUDEPATH += $$PWD/../../asdf_multiplat
DEPENDPATH += $$PWD/../../asdf_multiplat
