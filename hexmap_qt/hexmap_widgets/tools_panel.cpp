#include "tools_panel.h"

tools_panel_t::tools_panel_t(QWidget *parent)
    : QToolBar(tr("Tools"), parent)
{
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    namespace hxm_ed = asdf::hexmap::editor;
    using tools_types = hxm_ed::editor_t::tool_type_e;


    for(size_t i = 0; i < tools_types::num_tool_types; ++i)
    {
        tool_actions[i] = addAction(QIcon(), hxm_ed::tool_type_strings[i]);
    }
}

tools_panel_t::~tools_panel_t()
{
}
