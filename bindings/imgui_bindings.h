#pragma once 
#include "lua_binding.h"

namespace bindings {

	struct ImGuiBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;

	};

}