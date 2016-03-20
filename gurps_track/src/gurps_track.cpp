//  Copyright (c) 2015 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.

#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

#include <kiui/Ui/mkUi.h>

#include <kiui/Ui/Nano/mkGlWindow.h>

#include "data/character.h"
#include "ui/main_layout.h"
#include "ui/skill_list_layout.h"

#define ASSETS_PATH "../assets/"

using namespace mk;
using namespace gurps_track;
using namespace gurps_track::data;
using namespace gurps_track::ui;

int main(int argc, char *argv[])
{
	mk::GlWindow glwindow(500, 800, "kiUi demo DAS TEST", ASSETS_PATH);
	glwindow.initContext();

	mk::UiWindow& uiwindow = glwindow.uiWindow();
	uiwindow.init();

	mk::Form& root = uiwindow.rootForm();
	mk::StyleParser parser(uiwindow.styler());
	// parser.loadStyleSheet(root.sheet().uiWindow().resourcePath() + "interface/styles/blendish_dark.yml");
	
	Header& header = root.sheet().emplace<Header>();
	// Board& body = root.sheet().emplace<Board>();
	// LayerSheet& body = board.emplace<LayerSheet>();

	// todo: replace with icons
	header.emplace<Button>("New");
	header.emplace<Button>("Open");
	header.emplace<Button>("Save");
	header.emplace<Button>("Save As");

	{
		character_t test_char("");
		skill_list_t test_skills("../assets/data/gurps_skills.json");

		root.sheet().emplace<main_layout_t>(test_char);
		// root.sheet().emplace<skill_library_t>(test_skills, test_char);
	}

	bool pursue = true;
	while (pursue)
		pursue = glwindow.renderFrame();
}
