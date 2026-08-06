#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "layerus.h"

namespace bindings {

	struct LayerCollectionBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<LayerCollection> type;
	};

}