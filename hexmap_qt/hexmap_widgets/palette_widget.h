#ifndef PALLET_WIDGET_H
#define PALLET_WIDGET_H

#include <vector>

#include <QWidget>
#include <QAbstractListModel>
#include <QAbstractItemDelegate>

class QListView;

//namespace asdf::data
namespace asdf{
    namespace data
    {
        struct texture_atlas_t;
    }
    namespace hexmap {
        namespace data
        {
            struct terrain_bank_t;
        }
//        namespace editor
//        {
//            struct editor_t;
//        }
    }
}

namespace Ui {
class palette_widget;
}


class palette_item_model_t : public QAbstractListModel
{
    Q_OBJECT

public:
    palette_item_model_t(QObject* parent = 0);

    //QModelIndex index(int row, int column, const QModelIndex &parent) const;
    //QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    void build_from_terrain_bank(asdf::hexmap::data::terrain_bank_t const&);
    void build_from_atlas(asdf::data::texture_atlas_t const& atlas);


    struct entry_t
    {
        QString name;
        QImage thumbnail;
    };

    QVector<entry_t> entries;
};

class palette_delegate_t : QAbstractItemDelegate
{
    Q_OBJECT

public:

    explicit palette_delegate_t(QObject* parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class palette_widget_t : public QWidget
{
    Q_OBJECT

public:
    explicit palette_widget_t(QWidget *parent = 0);
    ~palette_widget_t();

    QModelIndex selected_index() const;

    void import_terrain();
    void remove_selected_terrain();
    void save_terrain();


signals:
    void terrain_add(QStringList const& terrain_filepaths);

public slots:
    void build_from_terrain_bank(asdf::hexmap::data::terrain_bank_t const&);
    void build_from_atlas(asdf::data::texture_atlas_t const& atlas);

public:
    QString default_texture_import_dir;
    QString default_texture_save_dir;

    QListView* list_view = nullptr; //store this publicly so other things can connect the signals it sends out

private:
    //void pressed(const QModelIndex&)

    Ui::palette_widget *ui;
};

#endif // PALLET_WIDGET_H
