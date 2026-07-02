#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "picking.h"

namespace bindings {

	struct PickingBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<PickResult> type;
		sol::usertype<PickState> pickStateType;
	};

}