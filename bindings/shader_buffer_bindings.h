#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "mesh_buffer.h"

namespace bindings {

	struct ShaderBufferBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<MeshBuffer> type;
	};

}