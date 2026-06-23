#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/scene/scene_node.h"

namespace bindings {

	struct SceneNodeBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<SceneNode> type;
	};

}