#pragma once 
#include "../lua_binding.h"
#include "../../core/app_interface.h"
#include "../../core/data/image.h"

namespace bindings {

	struct ImageBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override {

			type = lua.new_usertype<Image>("Image", 
				sol::constructors<Image()>(),
				"tex_id", sol::readonly_property(&Image::getTexId),
				"width", sol::readonly_property(&Image::getWidth),
				"height", sol::readonly_property(&Image::getHeight),
				"channels", sol::readonly_property(&Image::getChannels)
			);
			
		}

		sol::usertype<Image> type;
	};

}