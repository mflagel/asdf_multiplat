#ifndef NEW_MAP_DIALOG_H
#define NEW_MAP_DIALOG_H

#include <QDialog>

namespace Ui {
class new_map_dialog_t;
}

class new_map_dialog_t : public QDialog
{
    Q_OBJECT

public:
    explicit new_map_dialog_t(QWidget *parent = 0);
    ~new_map_dialog_t();

private:
    Ui::new_map_dialog_t *ui;

    friend class MainWindow;
};

#endif // NEW_MAP_DIALOG_H
