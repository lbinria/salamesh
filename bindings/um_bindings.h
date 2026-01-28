// Note: we only bind attributes from ultimaille
// It enable to manipulate attributes in lua
// The rest of ultimaille must be manipulated from a C++ script
#pragma once 
#include "lua_binding.h"



namespace bindings {

	struct UMBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;

	};

}