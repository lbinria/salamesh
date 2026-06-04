#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/scene.h"

namespace bindings {

	struct RenderSurfaceBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<RenderSurface> type;
	};

}