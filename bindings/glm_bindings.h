#pragma once 
#include "lua_binding.h"

namespace bindings {

	struct GlmBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;

	};
}