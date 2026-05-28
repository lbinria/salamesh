#include "material_params_bindings.h"

namespace bindings {

	void MaterialParamsBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<MaterialParams>("MaterialParams",
			"get", &MaterialParams::get,
			"set", &MaterialParams::set,
			sol::meta_function::index, &MaterialParams::get,
			sol::meta_function::new_index, &MaterialParams::set
		);
	}
}