#pragma once 
#include "lua_binding.h"

namespace bindings {

	struct CameraBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;

	};

}