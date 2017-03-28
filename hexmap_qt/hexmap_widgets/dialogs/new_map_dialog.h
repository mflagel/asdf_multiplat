#ifndef NEW_MAP_DIALOG_H
#define NEW_MAP_DIALOG_H

#include <QDialog>

namespace Ui {
class new_map_dialog_t;
}

namespace asdf { namespace hexmap { namespace data
{
    struct terrain_bank_t;
}}}


class new_map_dialog_t : public QDialog
{
    Q_OBJECT

public:
    explicit new_map_dialog_t(QWidget *parent = 0);
    ~new_map_dialog_t();

    void set_base_tiles(asdf::hexmap::data::terrain_bank_t const&);
    size_t selected_base_tile_index() const;

private:
    Ui::new_map_dialog_t *ui;

    friend class MainWindow;
};

#endif // NEW_MAP_DIALOG_H
