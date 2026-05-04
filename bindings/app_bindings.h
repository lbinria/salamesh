#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"

namespace bindings {

	struct AppBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;

	};

}