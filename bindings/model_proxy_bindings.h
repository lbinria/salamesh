#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/model_proxy.h"

namespace bindings {

	struct ModelProxyBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<ModelProxy> type;
	};

}