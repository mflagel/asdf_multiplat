#include "stdafx.h"

#include "gurps_track.h"
#include "test_kiui_layout.h"

using namespace std;
using namespace glm;

namespace asdf
{
    using namespace gurps_track;

    test_kiui_layout_t::test_kiui_layout_t()
    {
    	RootSheet& root = uiwindow.rootSheet();
    	Window& window = root.emplace<Window>("Example window");

    	Expandbox& expandbox = window.emplace<Expandbox>("Collapsable box");

    	Label& label = expandbox.emplace<Label>("kiUi says hello.");
    	Button& button = expand.emplace<Button>("Push me");
    }
}