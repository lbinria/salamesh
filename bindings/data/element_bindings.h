#pragma once 
#include "../lua_binding.h"
#include "../../core/app_interface.h"
#include "../../core/data/element.h"

namespace bindings {

	struct ElementBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override {

			lua.set_function("element_kind_to_string", &elementKindToString);
			lua.set_function("element_type_to_string", &elementTypeToString);
			
		}

		sol::usertype<Colormap> type;
	};

}