#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/shaders/material_params.h"

namespace bindings {

	struct MaterialParamsBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<MaterialParams> type;
	};

}