#include "clipping_mode_bindings.h"
#include "clipping_params.h"

namespace bindings {

	void ClippingModeBindings::loadBindings(sol::state &lua, IApp &app) {
		
		auto x = lua.new_enum("ClippingMode", 
			"STD", ClippingMode::STD,
			"CELL", ClippingMode::CELL
		);

		auto type = lua.new_usertype<ClippingParams>("ClippingParams", 
			"clipping_mode_strings", sol::readonly_property(&getClippingModeStrings)
		);

		
	}
}