#ifndef MAP_PROPERTIES_DIALOG_H
#define MAP_PROPERTIES_DIALOG_H

#include <QDialog>

namespace Ui {
class map_properties_dialog_t;
}

class map_properties_dialog_t : public QDialog
{
    Q_OBJECT

public:
    explicit map_properties_dialog_t(QWidget *parent = 0);
    ~map_properties_dialog_t();

private:
    Ui::map_properties_dialog_t *ui;
};

#endif // MAP_PROPERTIES_DIALOG_H
