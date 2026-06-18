#include "shader_buffer_bindings.h"

namespace bindings {

	void ShaderBufferBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<ShaderBuffer>("ShaderBuffer"

		);
	}
}