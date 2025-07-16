#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <glm/glm.hpp>

namespace bindings {

	struct GlmBindings {

		static void loadBindings(sol::state &lua, IApp &app) {

			// vec3 type
			sol::usertype<glm::vec3> vec3_type = lua.new_usertype<glm::vec3>("vec3",

				// Constructor overloads
				sol::constructors<glm::vec3(), 
								glm::vec3(float, float, float),
								glm::vec3(const glm::vec3&)>{},

				// Operators
				sol::meta_function::addition, [](const glm::vec3 &a, const glm::vec3 &b) {
					return a + b;
				},

				sol::meta_function::subtraction, [](const glm::vec3 &a, const glm::vec3 &b) {
					return a - b;
				},

				sol::meta_function::multiplication, [](const glm::vec3 &a, const glm::vec3 &b) {
					return a * b;
				},

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

			// TODO remove
			vec3_type.set_function("getX", [](glm::vec3 &v) {
				return v.x;
			});
		}

	};
}