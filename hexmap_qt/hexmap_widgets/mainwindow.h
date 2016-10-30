#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <hexmap/ui/hex_map.h>  //put before any QT stuff that will include openGL

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void set_scrollbar_stuff();

public slots:
    void scrollbar_changed();

private:
    Ui::MainWindow *ui;

    glm::vec2 base_camera_offset;
};

#endif // MAINWINDOW_H
