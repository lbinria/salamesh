#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/navigation_path.h"

namespace bindings {

	struct NavigationPathBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<NavigationPath> type;
	};

}