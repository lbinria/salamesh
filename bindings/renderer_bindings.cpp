#include "renderer_bindings.h"

#include "../core/shaders/shader_base.h"
#include "../core/shaders/point_shader.h"
#include "../core/shaders/halfedge_shader.h"
#include "../core/shaders/line_shader.h"

namespace bindings {

	void RendererBindings::loadBindings(sol::state &lua, IApp &app) {

		sol::usertype<ShaderBase> renderer_t = lua.new_usertype<ShaderBase>("ShaderBase"
			// "get_params", [](ShaderBase &self, const std::string &name) {
			// 	return self.getParams(name);
			// },
			// "__index", [](ShaderBase& self, const std::string& name) {
			// 	return self.getParams(name);
			// }
		);

		renderer_t["name"] = sol::readonly_property(&ShaderBase::getName);



		// TODO important missing cases
		renderer_t.set_function("as", [](sol::this_state s, ShaderBase& self, const std::string& typeName) -> sol::object {
			if (typeName == "LineShader") {
				if (auto r = dynamic_cast<LineShader*>(&self)) {
					return sol::make_object(s, r);
				}
			} else if (typeName == "PointShader") {
				if (auto r = dynamic_cast<PointShader*>(&self)) {
					return sol::make_object(s, r);
				}
			}

			return sol::nil;
		});



		sol::usertype<PointShader> pointSetRenderer_t = lua.new_usertype<PointShader>(
			"PointShader",
			sol::base_classes, 
			sol::bases<ShaderBase>(),
			"auto_update", sol::writeonly_property(&PointShader::setAutoUpdate),
			sol::meta_function::index, [](PointShader& self, int i) -> vec3 {
				return self[i - 1];
			},
			sol::meta_function::new_index, [](PointShader& self, int i, vec3 value) {
				self[i - 1] = value;
			},
			"count", sol::readonly_property(&PointShader::count)
		);


		sol::usertype<HalfedgeShader> halfedgeRenderer_t = lua.new_usertype<HalfedgeShader>(
			"HalfedgeShader",
			sol::base_classes, 
			sol::bases<ShaderBase>()
		);


		auto line_t = lua.new_usertype<LineShader::Line>(
			"Line",
			sol::call_constructor, [](sol::table t) {
				LineShader::Line line;
				
				if (t["a"].valid() && t["a"].is<glm::vec3>())
					line.a = t["a"].get<glm::vec3>();
				if (t["b"].valid() && t["b"].is<glm::vec3>())
					line.b = t["b"].get<glm::vec3>();
				if (t["color"].valid() && t["color"].is<glm::vec3>())
					line.color = t["color"].get<glm::vec3>();
				
				return line;
			},
			"a", &LineShader::Line::a,
			"b", &LineShader::Line::b,
			"color", &LineShader::Line::color
		);

		auto lineRenderer_t = lua.new_usertype<LineShader>(
			"LineShader",
			sol::base_classes, 
			sol::bases<ShaderBase>(),
			"auto_update", sol::writeonly_property(&LineShader::setAutoUpdate)
		);
	}
}