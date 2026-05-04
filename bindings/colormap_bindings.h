#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/data/colormap.h"

namespace bindings {

	struct ColormapBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<Colormap> type;
	};

}