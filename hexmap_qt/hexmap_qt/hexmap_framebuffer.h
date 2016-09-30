#ifndef HEXMAP_FRAMEBUFFER_H
#define HEXMAP_FRAMEBUFFER_H

#include <memory>

#include <QQuickFramebufferObject>
#include <QOpenGLFramebufferObject>


#include <hexmap/ui/hex_map.h>


// http://doc.qt.io/qt-5/qquickframebufferobject.html#details

// "All item logic, such as properties and UI-related helper functions
//  needed by QML should be located in [this class]"

using QtQuickFBO = QQuickFramebufferObject;

class hexmap_item : public QtQuickFBO
{
    QtQuickFBO::Renderer* createRenderer() const override;

public:
    hexmap_item() = default;
};


// http://doc.qt.io/qt-5/qquickframebufferobject-renderer.html#render

class hexmap_renderer_t : public QtQuickFBO::Renderer
{
    asdf::hexmap::data::hex_grid_t hex_grid;
    std::unique_ptr<asdf::hexmap::data::hex_grid_t> hex_map;

    hexmap_renderer_t();

    QOpenGLFramebufferObject* createFramebufferObject(const QSize &size) override;

    void render() override;
    void synchronize(QtQuickFBO *item) override;
};

#endif // HEXMAP_FRAMEBUFFER_H
