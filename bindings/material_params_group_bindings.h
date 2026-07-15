#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "material_group.h"

namespace bindings {

	struct MaterialParamsGroupBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<MaterialParamsGroup> type;
	};

}