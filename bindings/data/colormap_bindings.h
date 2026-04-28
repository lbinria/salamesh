#pragma once 
#include "../lua_binding.h"
#include "../../core/app_interface.h"
#include "../../core/data/colormap.h"

namespace bindings {

	struct ColormapBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override {

			type = lua.new_usertype<Colormap>("Colormap", 
				sol::constructors<Colormap()>(),
				"name", sol::readonly_property(&Colormap::name),
				"width", sol::readonly_property(&Colormap::width),
				"height", sol::readonly_property(&Colormap::height),
				"tex", sol::readonly_property(&Colormap::tex)
			);
			
		}

		sol::usertype<Colormap> type;
	};

}