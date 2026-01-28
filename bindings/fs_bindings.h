#pragma once 
#include "lua_binding.h"


namespace fs = std::filesystem;

namespace bindings {

	struct FsBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;

	};

}