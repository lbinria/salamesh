#include "material_params_group_bindings.h"

namespace bindings {

	void MaterialParamsGroupBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<MaterialParamsGroup>("MaterialParamsGroup",
			"get", &MaterialParamsGroup::get,
			"set", &MaterialParamsGroup::set,
			"get_bools", &MaterialParamsGroup::getBools
			// sol::meta_function::index, &MaterialParamsGroup::get,
			// sol::meta_function::new_index, &MaterialParamsGroup::set
		);
	}
}