#include "clipping_mode_bindings.h"

namespace bindings {

	void ClippingModeBindings::loadBindings(sol::state &lua, IApp &app) {
		
		auto x = lua.new_enum("ClippingMode", 
			"STD", ClippingMode::STD,
			"CELL", ClippingMode::CELL
		);

		lua.new_usertype<ClippingInfo>("ClippingInfo",
			"clipping_mode_strings", sol::property(
				[]() -> std::array<std::string_view, 2> {
					return {
						ClippingInfo::clippingModeStrings[0],
						ClippingInfo::clippingModeStrings[1]
					};
				}
			)
		);
	}
}