#include "render_surface_bindings.h"

namespace bindings {

	void RenderSurfaceBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<RenderSurface>("RenderSurface",
			"get_pick_state", &RenderSurface::getPickState
		);


	}
}