#include "renderer_bindings.h"

#include "../core/renderers/renderer.h"
#include "../core/renderers/mesh_renderer.h"
#include "../core/renderers/point_set_renderer.h"
#include "../core/renderers/halfedge_renderer.h"
#include "../core/renderers/line_renderer.h"

namespace bindings {

	void RendererBindings::loadBindings(sol::state &lua, IApp &app) {

		sol::usertype<Material> renderer_t = lua.new_usertype<Material>("Material",
			"get_params", &Material::getParams,
			"__index", [](Material& self, const std::string& name) {
				return self.getParams(name);
			}
		);

		renderer_t["name"] = sol::readonly_property(&Material::getName);

		renderer_t["visible"] = sol::property(
			&Material::getVisible,
			&Material::setVisible
		);

		renderer_t["light"] = sol::writeonly_property(&Material::setLight);


		// TODO important missing cases
		renderer_t.set_function("as", [](sol::this_state s, Material& self, const std::string& typeName) -> sol::object {
			if (typeName == "LineMaterial") {
				if (auto r = dynamic_cast<LineMaterial*>(&self)) {
					return sol::make_object(s, r);
				}
			} else if (typeName == "PointMaterial") {
				if (auto r = dynamic_cast<PointMaterial*>(&self)) {
					return sol::make_object(s, r);
				}
			}

			return sol::nil;
		});

		renderer_t.set_function("push", &Material::push);


		sol::usertype<MeshMaterial> meshRenderer_t = lua.new_usertype<MeshMaterial>(
			"MeshMaterial",
			sol::base_classes, 
			sol::bases<Material>()
		);

		meshRenderer_t["color"] = sol::property(
			&MeshMaterial::getColor,
			&MeshMaterial::setColor
		);

		meshRenderer_t["size"] = sol::property(
			&MeshMaterial::getMeshSize,
			&MeshMaterial::setMeshSize
		);

		meshRenderer_t["shrink"] = sol::property(
			&MeshMaterial::getMeshShrink,
			&MeshMaterial::setMeshShrink
		);

		meshRenderer_t["corner_visible"] = sol::property(
			&MeshMaterial::getCornerVisible,
			&MeshMaterial::setCornerVisible
		);


		sol::usertype<PointMaterial> pointSetRenderer_t = lua.new_usertype<PointMaterial>(
			"PointMaterial",
			sol::base_classes, 
			sol::bases<Material>(),
			"auto_update", sol::writeonly_property(&PointMaterial::setAutoUpdate),
			"visible", sol::property(
				&PointMaterial::getVisible,
				&PointMaterial::setVisible
			),
			sol::meta_function::index, [](PointMaterial& self, int i) -> vec3 {
				return self[i - 1];
			},
			sol::meta_function::new_index, [](PointMaterial& self, int i, vec3 value) {
				self[i - 1] = value;
			},
			"add_point", &PointMaterial::addPoint,
			"add_points", &PointMaterial::addPoints,
			"remove_points", &PointMaterial::removePoints,
			"clear_points", &PointMaterial::clearPoints,
			"push", &PointMaterial::push,
			"count", sol::readonly_property(&PointMaterial::count)
		);


		// auto materialParams = lua.new_usertype<MaterialParams>("MaterialParams"
		// 	"get", &MaterialParams::get,
		// 	"set", &MaterialParams::set
		// );
		auto materialParams = lua.new_usertype<MaterialParams>("MaterialParams",
			"get", sol::overload(
				static_cast<ParamValue (MaterialParams::*)(const std::string)>(&MaterialParams::get),
				static_cast<ParamValue (MaterialParams::*)(const std::string, int)>(&MaterialParams::get)
			),
			"set", sol::overload(
				static_cast<void (MaterialParams::*)(const std::string, ParamValue)>(&MaterialParams::set),
				static_cast<void (MaterialParams::*)(const std::string, int, ParamValue)>(&MaterialParams::set)
			),
			"__index", [](MaterialParams& self, const std::string& name) {
				return self.get(name);
			},
			"__newindex", [](MaterialParams& self, const std::string& name, ParamValue value) {
				self.set(name, value);
			}
		);


		sol::usertype<HalfedgeMaterial> halfedgeRenderer_t = lua.new_usertype<HalfedgeMaterial>(
			"HalfedgeMaterial",
			sol::base_classes, 
			sol::bases<Material>()
		);

		halfedgeRenderer_t["visible"] = sol::property(
			&HalfedgeMaterial::getVisible,
			&HalfedgeMaterial::setVisible
		);
		halfedgeRenderer_t["inside_color"] = sol::property(
			&HalfedgeMaterial::getInsideColor,
			&HalfedgeMaterial::setInsideColor
		);
		halfedgeRenderer_t["outside_color"] = sol::property(
			&HalfedgeMaterial::getOutsideColor,
			&HalfedgeMaterial::setOutsideColor
		);
		halfedgeRenderer_t["thickness"] = sol::property(
			&HalfedgeMaterial::getThickness,
			&HalfedgeMaterial::setThickness
		);
		halfedgeRenderer_t["spacing"] = sol::property(
			&HalfedgeMaterial::getSpacing,
			&HalfedgeMaterial::setSpacing
		);
		halfedgeRenderer_t["padding"] = sol::property(
			&HalfedgeMaterial::getPadding,
			&HalfedgeMaterial::setPadding
		);


		auto line_t = lua.new_usertype<LineMaterial::Line>(
			"Line",
			sol::call_constructor, [](sol::table t) {
				LineMaterial::Line line;
				
				if (t["a"].valid() && t["a"].is<glm::vec3>())
					line.a = t["a"].get<glm::vec3>();
				if (t["b"].valid() && t["b"].is<glm::vec3>())
					line.b = t["b"].get<glm::vec3>();
				if (t["color"].valid() && t["color"].is<glm::vec3>())
					line.color = t["color"].get<glm::vec3>();
				
				return line;
			},
			"a", &LineMaterial::Line::a,
			"b", &LineMaterial::Line::b,
			"color", &LineMaterial::Line::color
		);

		auto lineRenderer_t = lua.new_usertype<LineMaterial>(
			"LineMaterial",
			sol::base_classes, 
			sol::bases<Material>(),
			"auto_update", sol::writeonly_property(&LineMaterial::setAutoUpdate)
		);

		lineRenderer_t.set_function("add_line", &LineMaterial::addLine);
		lineRenderer_t.set_function("add_lines", &LineMaterial::addLines);
		lineRenderer_t.set_function("clear_lines", &LineMaterial::clearLines);
		lineRenderer_t.set_function("push", &LineMaterial::push);

		// auto pointSetRenderer_t = lua.new_usertype<PointMaterial>(
		// 	"PointMaterial",
		// 	sol::base_classes, 
		// 	sol::bases<Material>(),
		// 	"auto_update", sol::writeonly_property(&PointMaterial::setAutoUpdate)
		// );

		// pointSetRenderer_t.set_function("push", &PointMaterial::push);
		// pointSetRenderer_t.set_function("clear_points", &PointMaterial::clearPoints);
		// pointSetRenderer_t.set_function("add_point", &PointMaterial::addPoint);
		// pointSetRenderer_t.set_function("add_points", &PointMaterial::addPoints);

	}
}