#include "image_bindings.h"

namespace bindings {

	void ImageBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<Image>("Image", 
			sol::constructors<Image()>(),
			"tex_id", sol::readonly_property(&Image::getTexId),
			"width", sol::readonly_property(&Image::getWidth),
			"height", sol::readonly_property(&Image::getHeight),
			"channels", sol::readonly_property(&Image::getChannels)
		);
	}
}