#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/scene.h"

namespace bindings {

	struct SceneBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<Scene> type;
	};

}