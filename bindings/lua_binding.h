#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h" // TODO remove

namespace bindings {

	struct LuaBinding {

		virtual void loadBindings(sol::state &lua, IApp &app) = 0;

	};

}