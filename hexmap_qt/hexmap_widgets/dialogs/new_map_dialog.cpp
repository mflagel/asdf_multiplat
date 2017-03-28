#include "new_map_dialog.h"
#include "ui_new_map_dialog.h"

#include <QListView>

#include <hexmap/data/terrain_bank.h>

#include "palette_widget.h"

//todo: move to some "constants.h"?
namespace
{
    constexpr const char* map_name_placeholder = "Map Name";
    constexpr int max_map_width  = 1000;
    constexpr int max_map_height = 1000;
}

new_map_dialog_t::new_map_dialog_t(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::new_map_dialog_t)
{
    ui->setupUi(this);

    ui->txt_mapName->setPlaceholderText(QString(map_name_placeholder));

    ui->spb_width-> setMinimum(0);
    ui->spb_height->setMinimum(0);

    ui->spb_width-> setMaximum(max_map_width);
    ui->spb_height->setMaximum(max_map_height);
}

new_map_dialog_t::~new_map_dialog_t()
{
    delete ui;
}

void new_map_dialog_t::set_base_tiles(asdf::hexmap::data::terrain_bank_t const& terrain_bank)
{
    //use the same model as the palette_widget
    auto* model = new palette_item_model_t(this);
    model->build_from_terrain_bank(terrain_bank);
    ui->base_tile_palette->build_from_terrain_bank(terrain_bank);

    auto ind = model->index(0,0);
    if(ind.isValid())
        ui->base_tile_palette->list_view->selectionModel()->select(ind, QItemSelectionModel::Select);
}

size_t new_map_dialog_t::selected_base_tile_index() const
{
    ASSERT(ui->base_tile_palette->selected_index().row() >= 0, "expecting a non-negative selected tile");
    auto ind = ui->base_tile_palette->selected_index();

    if(ind.isValid())
        return static_cast<size_t>(ui->base_tile_palette->selected_index().row());
    else
        return 0; //default to 0th terrain type
}
