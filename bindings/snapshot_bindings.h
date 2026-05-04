#pragma once 

#include "lua_binding.h"

#include "../core/app_interface.h"
#include "../core/data/snapshot.h"

namespace bindings {

	struct SnapshotBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override;
		private:
		sol::usertype<Snapshot> type;
	};

}