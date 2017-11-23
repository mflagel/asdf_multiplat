#ifndef TOOLS_PANEL_H
#define TOOLS_PANEL_H

#include <QToolBar>
#include <hexmap/editor/main/editor.h>


class tools_panel_t : public QToolBar
{
    Q_OBJECT

public:
    explicit tools_panel_t(QWidget* parent = 0);
    ~tools_panel_t();

    std::array<QAction*,asdf::hexmap::editor::editor_t::num_tool_types> tool_actions;

private:
    friend class MainWindow;
};

#endif // TOOLS_PANEL_H
