#include "palette_widget.h"
#include "ui_palette_widget.h"

#include <QPainter>

#include "mainwindow.h"

#include "hexmap/data/terrain_bank.h"
#include "hexmap/editor/main/editor.h"

palette_widget_t::palette_widget_t(QWidget* parent)
: QWidget(parent)
, ui(new Ui::palette_widget)
{
    ui->setupUi(this);
    list_view = ui->listView;
}

palette_widget_t::~palette_widget_t()
{
    delete ui;
}

void palette_widget_t::build_from_terrain_bank(asdf::hexmap::data::terrain_bank_t const& terrain)
{
    auto* model = new palette_item_model_t(this);
    model->build_from_terrain_bank(terrain);
    ui->listView->setModel(model);
}

void palette_widget_t::build_from_atlas(asdf::data::texture_atlas_t const& atlas)
{
    auto* model = new palette_item_model_t(this);
    model->build_from_atlas(atlas);
    ui->listView->setModel(model);
}


palette_delegate_t::palette_delegate_t(QObject* parent)
: QAbstractItemDelegate(parent)
{

}

void palette_delegate_t::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    auto* entry = static_cast<palette_item_model_t::entry_t*>(index.internalPointer());
    if(!entry)
        return;

    auto name = index.data(Qt::DisplayRole).value<QString>();
    auto thumbnail = index.data(Qt::DecorationRole).value<QImage>();

    painter->save();

    painter->fillRect(option.rect, QBrush(thumbnail));

    painter->restore();
}

QSize palette_delegate_t::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize();
}



palette_item_model_t::palette_item_model_t(QObject *parent)
: QAbstractListModel(parent)
{
}

void palette_item_model_t::build_from_terrain_bank(asdf::hexmap::data::terrain_bank_t const& terrain)
{
    entries.clear();
    entries.reserve(terrain.saved_textures.size());

    for(size_t i = 0; i < terrain.saved_textures.size(); ++i)
    {
        palette_item_model_t::entry_t entry {
              terrain.asset_names[i].c_str()
            , QImage(terrain.saved_textures[i].filesystem_location.c_str())
        };

        entries.append(std::move(entry));
    }
}

void palette_item_model_t::build_from_atlas(asdf::data::texture_atlas_t const& atlas)
{
    entries.clear();
    entries.reserve(atlas.atlas_entries.size());

    // Create thumbnails by loading the atlas' texture into a QImage and copying out each atlas entry
    // It might be more efficient to render each entry onto new QImages from the atlas texture, since its already loaded
    // however it might be more effort than necessary
    QImage atlas_image( QString(atlas.texture_filepath.c_str()) );

    for(auto const& ae : atlas.atlas_entries)
    {
        auto top_left  = QPoint(ae.top_left_px.x, ae.top_left_px.y);
        auto size      = QSize(ae.size_px.x, ae.size_px.x);
        QRect rect(top_left, size);

        palette_item_model_t::entry_t entry {
              ae.filename.c_str() //TODO give names to atlas entries
            , atlas_image.copy(rect)
        };

        entries.append(std::move(entry));
    }
}

int palette_item_model_t::rowCount(const QModelIndex &parent) const
{
    return entries.size();
}

QVariant palette_item_model_t::data(const QModelIndex &index, int role) const
{
    auto const& entry = entries[index.row()];

    if (role == Qt::DisplayRole)
    {
       return entry.name;
    }
    else if(role == Qt::DecorationRole)
    {
        return entry.thumbnail;
    }

    return QVariant();
}
