#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QQuickView>
#include <QGLFormat>

#include <memory>

#include <asdf_multiplat/main/asdf_multiplat.h>
#include <asdf_multiplat/data/content_manager.h>

//constexpr int scroll_sub_ticks = 10;

using namespace std;
using namespace glm;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // http://stackoverflow.com/a/33074982
    QGLFormat glFormat;
    glFormat.setVersion(3, 2);
    glFormat.setProfile(QGLFormat::CoreProfile);
    QGLFormat::setDefaultFormat(glFormat);

    ui->setupUi(this);

    // give hexmap widget a pointer to this, so it can set up scrollbar ranges after initializeGL is called
    // can't do it now because we need the hex_map to be created, which requires an openGL context that hasnt
    // been initialized yet
    ui->hexmap_widget->main_window = this;


    connect(ui->hexmap_hscroll, &QScrollBar::valueChanged, this, &MainWindow::scrollbar_changed);
    connect(ui->hexmap_vscroll, &QScrollBar::valueChanged, this, &MainWindow::scrollbar_changed);

    //auto tools_panel = new ToolsPa
    //ui->left_column->addWidget();

    // http://www.ics.com/blog/combining-qt-widgets-and-qml-qwidgetcreatewindowcontainer
//    QQuickView *view = new QQuickView();
//    QWidget *container = QWidget::createWindowContainer(view, this);
//    container->setMinimumSize(200, 200);
//    container->setMaximumSize(200, 200);
//    container->setFocusPolicy(Qt::TabFocus);
//    view->setSource(QUrl("main.qml"));
//    ui->verticalLayout->addWidget(container);


}

MainWindow::~MainWindow()
{
    delete ui;
}

/// TODO: refactor this such that most of the math is done in the hexmap widget
///       and the widget then calls this function with scrollbar ranges as params
void MainWindow::set_scrollbar_stuff()
{
    hexmap_widget_t* hxm_wgt = ui->hexmap_widget;

    //set scroll bar info from hex_map

    //figure out how many units can fit on screen
    vec2 hex_size(asdf::hexmap::hex_width, asdf::hexmap::hex_height);
    auto const& map_size = hxm_wgt->map_size(); //array size
    vec2 map_size_units = vec2(map_size) * hex_size; //total map width in units. Width is not equal to array width because hexagons overlap, and hexagons aren't one unit tall
    map_size_units.x -= (map_size.x - 1) * asdf::hexmap::hex_width_d4; //handle horizontal overlap

    //get size of a hex in pixels. hex_width and hex_height constexpr constants exist in ui/hex_map.h
    vec2 hx_size_px = hex_size * hxm_wgt->camera_zoom();
    vec2 map_size_px = map_size_units * hx_size_px;

    base_camera_offset = glm::vec2(map_size_units) / 4.0f;
    hxm_wgt->camera_pos(base_camera_offset);

    auto* h_scr = ui->hexmap_hscroll;
    h_scr->setMinimum(0);
    h_scr->setMaximum(map_size_units.x);
    h_scr->setPageStep(map_size_px.x / width());

    auto* v_scr = ui->hexmap_hscroll;
    v_scr->setMinimum(0);
    v_scr->setMaximum(map_size_units.y);
    v_scr->setPageStep(map_size_px.y / height());
}


void MainWindow::scrollbar_changed()
{
    LOG("scrollbar changed;  x:%d  y%d", ui->hexmap_hscroll->value(), ui->hexmap_vscroll->value());

    glm::vec2 p(ui->hexmap_hscroll->value(), -ui->hexmap_vscroll->value());
    p += base_camera_offset;

    ui->hexmap_widget->camera_pos(p);
    ui->hexmap_widget->update();
}
