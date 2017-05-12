#ifndef EXPORT_IMAGE_DIALOG_H
#define EXPORT_IMAGE_DIALOG_H

#include <QDialog>

namespace Ui {
class export_image_dialog_t;
}

class export_image_dialog_t : public QDialog
{
    Q_OBJECT

public:
    explicit export_image_dialog_t(QWidget *parent = 0);
    ~export_image_dialog_t();

private:
    Ui::export_image_dialog_t *ui;
};

#endif // EXPORT_IMAGE_DIALOG_H
