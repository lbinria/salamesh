#include "material_bindings.h"

namespace bindings {

	void MaterialBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<Material>("Material",
			"set", &Material::set,
			"params", sol::readonly_property(
				[](Material& self) {
					return self.getParams();
				}
			),
			"get_params", sol::overload(
				[](Material &self, const std::string name) {
					return self.getParams(name);
				}, 
				[](Material &self) {
					return self.getParams();
				}
			),
			"visible", sol::property(&Material::isVisible, &Material::setVisible),
			sol::meta_function::index, [](Material &self, const std::string name) {
				return self.getParams(name);
			}
		);
	}
}