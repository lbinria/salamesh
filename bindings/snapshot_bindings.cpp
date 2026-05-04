#include "snapshot_bindings.h"

namespace bindings {

	void SnapshotBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<Snapshot>("Snapshot", 
			sol::constructors<Snapshot()>(),
			"state_filename", sol::readonly_property(&Snapshot::getStateFilename),
			"thumb_filename", sol::readonly_property(&Snapshot::getThumbfilename),
			"image", sol::readonly_property(&Snapshot::getImage)
		);
	}
}