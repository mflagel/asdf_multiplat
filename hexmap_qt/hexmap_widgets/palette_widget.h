#ifndef PALLET_WIDGET_H
#define PALLET_WIDGET_H

#include <vector>

#include <QWidget>

//namespace asdf::data
namespace asdf{ namespace data
{
    struct texture_bank_t;
}}

namespace Ui {
class palette_widget;
}

class palette_entry_t : public QWidget
{
    Q_OBJECT
};

class palette_widget : public QWidget
{
    Q_OBJECT

public:
    explicit palette_widget(QWidget *parent = 0);
    ~palette_widget();

    void build_from_texture_bank(asdf::data::texture_bank_t const&);

private:
    Ui::palette_widget *ui;

    std::vector<palette_entry_t> entries;
};

#endif // PALLET_WIDGET_H
