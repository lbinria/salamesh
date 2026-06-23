#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "geometry.h"

namespace bindings {

	struct GeometryBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<Geometry> type;
	};

}