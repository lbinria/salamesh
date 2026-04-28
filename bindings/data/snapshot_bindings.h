#pragma once 
#include "../lua_binding.h"
#include "../../core/app_interface.h"
#include "../../core/data/snapshot.h"

namespace bindings {

	struct SnapshotBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override {

			type = lua.new_usertype<Snapshot>("Snapshot", 
				sol::constructors<Snapshot()>(),
				"state_filename", sol::readonly_property(&Snapshot::getStateFilename),
				"thumb_filename", sol::readonly_property(&Snapshot::getThumbfilename),
				"image", sol::readonly_property(&Snapshot::getImage)
			);
			
		}

		sol::usertype<Snapshot> type;
	};

}