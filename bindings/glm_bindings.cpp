#include "glm_bindings.h"

#include <glm/glm.hpp>

namespace bindings {

	void GlmBindings::loadBindings(sol::state &lua, IApp &app) {

		// vec3 type
		sol::usertype<glm::vec3> vec3_type = lua.new_usertype<glm::vec3>("vec3",

			// Constructor overloads
			sol::constructors<glm::vec3(), 
							glm::vec3(float, float, float),
							glm::vec3(const glm::vec3&)>{},

			sol::call_constructor, [](sol::table t) {
				return glm::vec3{
					t.get_or(1, 0.f),
					t.get_or(2, 0.f),
					t.get_or(3, 0.f)
				};
			},

			sol::meta_function::addition, sol::overload(
				[](const glm::vec3 &a, const glm::vec3 &b) {
					return a + b;
				}/*, 
				[](const float a, const glm::vec3 &b) {
					return a + b;
				}*/
			),

			sol::meta_function::subtraction, [](const glm::vec3 &a, const glm::vec3 &b) {
				return a - b;
			},

			sol::meta_function::multiplication, sol::overload(
				[](const glm::vec3 &a, const glm::vec3 &b) {
					return a * b;
				}, 
				[](const float a, const glm::vec3 &b) {
					return a * b;
				}
			),

			sol::meta_function::division, [](const glm::vec3 &a, const glm::vec3 &b) {
				return a / b;
			},
			
			// Property accessors
			"x", sol::readonly_property(&glm::vec3::x),
			"y", sol::readonly_property(&glm::vec3::y),
			"z", sol::readonly_property(&glm::vec3::z)
		);

		vec3_type.set_function("to_string", [](glm::vec3 &v) {
			return std::string("(") + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
		});

	}
}