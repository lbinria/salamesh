#include "glm_bindings.h"

namespace bindings {

	void GlmBindings::loadBindings(sol::state &lua, IApp &app) {

		// vec2 type
		sol::usertype<sl::algebra::vec2> vec2_type2 = lua.new_usertype<sl::algebra::vec2>("vec2",

			sol::call_constructor, [](sol::table t) {
				return sl::algebra::vec2{
					t.get_or(1, 0.f),
					t.get_or(2, 0.f)
				};
			},
			
			// Property accessors
			"x", sol::readonly_property(&sl::algebra::vec2::x),
			"y", sol::readonly_property(&sl::algebra::vec2::y),
			"to_string", [](sl::algebra::vec2 &v) {
				return std::string("(") + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
			}
		);

		// vec3 type
		sol::usertype<sl::algebra::vec3> vec3_type2 = lua.new_usertype<sl::algebra::vec3>("vec3",

			sol::call_constructor, [](sol::table t) {
				return sl::algebra::vec3{
					t.get_or(1, 0.f),
					t.get_or(2, 0.f),
					t.get_or(3, 0.f)
				};
			},
			
			// Property accessors
			"x", sol::readonly_property(&sl::algebra::vec3::x),
			"y", sol::readonly_property(&sl::algebra::vec3::y),
			"z", sol::readonly_property(&sl::algebra::vec3::z),
			"to_string", [](sl::algebra::vec3 &v) {
				return std::string("(") + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
			}
		);

	}
}