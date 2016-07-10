#pragma once

#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

namespace gurps_track
{
namespace ui
{
namespace create
{

	/*
		+-----------------------+
		|- Trait Name [12]  [X]-|
		|    Stat +1            |
		|    Description        |
		+-----------------------+


		+-----------------------+
		| Name:  ______________ |
		| Point Cost:  ___ [+|-]|
		|   Status Effect 1
		| Description:          |
		|  ____________________ |
		+-----------------------+
	 */
	struct trait_t : mk::Sheet
	{
	};

}
}
}