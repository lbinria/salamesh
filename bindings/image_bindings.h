#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/data/image.h"

namespace bindings {

	struct ImageBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<Image> type;
	};

}