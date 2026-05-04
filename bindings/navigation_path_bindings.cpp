#include "navigation_path_bindings.h"

namespace bindings {

	void NavigationPathBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<NavigationPath>("NavigationPath",
			"get", &NavigationPath::get,
			"str", &NavigationPath::str,
			"starts_with", sol::overload(
				[](NavigationPath &self, std::vector<std::string> head) { self.startsWith(head); },
				[](NavigationPath &self, std::string head) { self.startsWith(head); }
			),
			sol::meta_function::to_string, [](NavigationPath& self) {
				return self.str();
			}//,
			// sol::meta_function::equal_to, [](const NavigationPath& self, const sol::object& other) {
			// 	if (other.is<std::string>()) {
			// 		return self.str() == other.as<std::string>();
			// 	} else if (other.is<NavigationPath>()) {
			// 		return self == other.as<NavigationPath>();
			// 	}
			// 	return false;
			// }
		);
	}
}