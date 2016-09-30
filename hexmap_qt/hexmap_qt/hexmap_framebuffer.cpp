#include "hexmap_framebuffer.h"

#include <QQuickWindow>


// implementations based on (or copied from):
// https://blog.qt.io/blog/2015/05/11/integrating-custom-opengl-rendering-with-qt-quick-via-qquickframebufferobject/

QtQuickFBO::Renderer* hexmap_item::createRenderer() const
{
    return new hexmap_renderer_t;
}


//
//
hexmap_renderer_t::hexmap_renderer_t()
: hex_grid(ivec2(15, 16))
{
    //hex_map = make_unique<ui::hex_map_t>(hex_grid);
}


QOpenGLFramebufferObject* hexmap_renderer_t::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);  //pretty sure I don't need a depth or stencil buffer for hexmap
    // optionally enable multisampling by doing format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void hexmap_renderer_t::render()
{
    //docs say that the FBO and viewport will be set to render to by this point

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0.7, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);


    //m_item->window()->resetOpenGLState(); // prevents code in this func from mucking up Qt Quick stuff

    //fbo will automatically unbind
}


void hexmap_renderer_t::synchronize(QQuickFramebufferObject *item)
{

}
