#include "colormap_bindings.h"

namespace bindings {

	void ColormapBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<Colormap>("Colormap", 
			sol::constructors<Colormap()>(),
			"name", sol::readonly_property(&Colormap::name),
			"width", sol::readonly_property(&Colormap::width),
			"height", sol::readonly_property(&Colormap::height),
			"tex", sol::readonly_property(&Colormap::tex)
		);
	}
}