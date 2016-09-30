#ifndef HEXMAP_QT_RENDERER_H
#define HEXMAP_QT_RENDERER_H

#include <QObject>
#include <QQuickItem>
#include <QOpenGLShaderProgram>
#include <QQuickWindow>
#include <QOpenGLFunctions>

class hexmap_qt_renderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
    public:
        hexmap_qt_renderer() : m_t(0), m_program(0) { }
        ~hexmap_qt_renderer();

        void setT(qreal t) { m_t = t; }
        void setViewportSize(const QSize &size) { m_viewportSize = size; }
        void setWindow(QQuickWindow *window) { m_window = window; }

    public slots:
        void paint();

    private:
        QSize m_viewportSize;
        qreal m_t;
        QOpenGLShaderProgram *m_program;
        QQuickWindow *m_window;
};




class hexmap_qt_item : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    hexmap_qt_item();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_t;
    hexmap_qt_renderer *m_renderer;

};

#endif // HEXMAP_QT_RENDERER_H
