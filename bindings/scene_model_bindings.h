#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/scene/scene_model.h"

namespace bindings {

	struct SceneModelBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<SceneModel> type;
	};

}