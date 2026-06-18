#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/material.h"

namespace bindings {

	struct MaterialBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<Material> type;
	};

}