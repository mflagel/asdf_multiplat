#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QQuickView>
#include <QGLFormat>
#include <QListView>

#include <memory>

#include <asdf_multiplat/main/asdf_multiplat.h>
#include <asdf_multiplat/data/content_manager.h>

#include <glm/gtc/matrix_transform.hpp>

#include "palette_widget.h"

using namespace std;
using namespace glm;


namespace
{
    constexpr int scroll_sub_ticks = 10;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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

    auto* palette_widget = new palette_widget_t(this);

    palette_widget->setMinimumSize(200, 300);

    connect(ui->hexmap_widget, &hexmap_widget_t::hex_map_initialized,
               palette_widget, &palette_widget_t::hex_map_initialized);
    connect(palette_widget->list_view, SIGNAL(pressed(const QModelIndex&)),
                    ui->hexmap_widget, SLOT(set_palette_item(const QModelIndex&)));

    ui->right_dock->layout()->addWidget(palette_widget);


}

MainWindow::~MainWindow()
{
    delete ui;
}


/*
   ________________
  |   entire map   |
  | ________       |
  ||        |      |
  || screen |      |
  ||________|      |
  |________________|
            ^------^ scroll range
*/


/// TODO: refactor this such that most of the math is done in the hexmap widget
///       and the widget then calls this function with scrollbar ranges as params
void MainWindow::set_scrollbar_stuff()
{
    hexmap_widget_t* hxm_wgt = ui->hexmap_widget;

    vec2 hex_size(asdf::hexmap::hex_width, asdf::hexmap::hex_height);

    auto const& map_size = hxm_wgt->map_size(); //array size
    vec2 map_size_units = vec2(map_size) * hex_size; //total map width in units. Width is not equal to array width because hexagons overlap, and hexagons aren't one unit tall
    map_size_units.x -= (map_size.x - 1) * asdf::hexmap::hex_width_d4; //handle horizontal overlap
    map_size_units.y += asdf::hexmap::hex_height_d2; //add room for the offset

    //get size of a hex in pixels. hex_width and hex_height constexpr constants exist in ui/hex_map.h
    vec2 hx_size_px = hex_size * hxm_wgt->camera_zoom();
    vec2 map_size_px = map_size_units * hx_size_px;

    base_camera_offset = glm::vec2();
    base_camera_offset.x = width()/2 / asdf::hexmap::px_per_unit;
    base_camera_offset.y = map_size_units.y - (height()/2 / asdf::hexmap::px_per_unit);
    hxm_wgt->camera_pos(base_camera_offset);

    //range is equal to how many units fit on the screen minus total map size in units

    //map_size_screen_px = unproject map_size_units from camera?

    glm::vec2 wgt_size(hxm_wgt->width(), hxm_wgt->height());


    auto hxm_proj = hxm_wgt->hex_map->camera.projection_ortho();

    vec4 vp;
    vp.x = 0.0f;
    vp.y = 0.0f;
    vp.z = wgt_size.x;
    vp.w = wgt_size.y;
    auto unprojd = unProject(vec3(wgt_size, 0.0f), glm::mat4(), hxm_proj, vp);

    auto screen_size_units = glm::vec2(unprojd);
    auto scr_range_units = glm::vec2(map_size_units) - glm::vec2(screen_size_units);

    auto* h_scr = ui->hexmap_hscroll;
    h_scr->setMinimum(0);
    h_scr->setMaximum(scr_range_units.x * scroll_sub_ticks);
    h_scr->setPageStep(screen_size_units.x * scroll_sub_ticks);

    auto* v_scr = ui->hexmap_hscroll;
    v_scr->setMinimum(0);
    v_scr->setMaximum(scr_range_units.y * scroll_sub_ticks);
    v_scr->setPageStep(screen_size_units.y * scroll_sub_ticks);

    scrollbar_changed();
}


void MainWindow::scrollbar_changed()
{
    //LOG("scrollbar changed;  x:%d  y%d", ui->hexmap_hscroll->value(), ui->hexmap_vscroll->value());

    glm::vec2 p(ui->hexmap_hscroll->value(), -ui->hexmap_vscroll->value());
    p /= scroll_sub_ticks;
    p += base_camera_offset;

    ui->hexmap_widget->camera_pos(p);
    ui->hexmap_widget->update();
}


void MainWindow::mouseMoveEvent(QMouseEvent* event)
{

}

void MainWindow::mousePressEvent(QMouseEvent* event)
{

}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{

}
