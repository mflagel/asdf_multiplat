#pragma once

#include <kiui/Ui/mkUiConfig.h>
#include <kiui/Ui/mkUiTypes.h>

namespace gurps_track
{

namespace data
{
	struct character_t;
	struct trait_t;
}

namespace ui
{
namespace character
{
	struct traits_t;



	/*
	+-----------------------+
	| Effect Name   Mod Amt |
	+-----------------------+
	 */
	// struct status_effect_entry_t : mk::Label
	// {
		
	// };


	/*
	+-----------------------+
	|- Trait Name [12]  [X]-|
	|    Status Effect 1    |
	|    Status Effect 2    |
	|    Description        |
	+-----------------------+
	 */
	struct trait_entry_t : mk::Expandbox
	{
		mk::Label* name;
		mk::Label* cost;
		mk::Label* description;

		std::vector<mk::Label*> modifiers;

		traits_t& parent;
		size_t index;

		trait_entry_t(traits_t&, size_t index);
		void set_data(data::trait_t const&);
	};


	/** Traits **
	+--------------------+
	|-       Elf        -|
	|- Perfect Balance  -|
	|- Attractive       -|
	|- Perfect Balance  -|
	|   ..               |
	|- Code of Honor    -|
	|- Sense of Duty    -|
	+--------------------+
	|-    Advantages    -|
	|-  Acute Vision +3  |
	|    Vision +3       |
	|    Description     |
	|- Magery 2          |
	|    Magery +2       |
	|-  Disadvantages   -|
	|-  Weak Will 2     -|
	|     Will -2        |
	|-  Overconfidence  -|
	|     Discription... |
	|-      Quirks      -|
	|    Quirk 1         |
	|    Quirk 2         |
	|    etc...          |
	+--------------------+
	 */
	struct traits_t : mk::Board
	{
		mk::List* list;

		data::character_t& character;

		traits_t(data::character_t&);
		void rebuild();
	};

}
}
}