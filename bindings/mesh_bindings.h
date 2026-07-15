#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "mesh.h"

namespace bindings {

	struct MeshBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<Mesh> type;
	};

}