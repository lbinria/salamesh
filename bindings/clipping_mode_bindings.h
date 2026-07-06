#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "clipping_mode.h"

namespace bindings {

	struct ClippingModeBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<ClippingMode> type;
	};

}