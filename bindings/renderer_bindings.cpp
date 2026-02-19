#include "renderer_bindings.h"

#include "../core/renderers/renderer.h"
#include "../core/renderers/point_set_renderer.h"
#include "../core/renderers/halfedge_renderer.h"
#include "../core/renderers/line_renderer.h"

namespace bindings {

	void RendererBindings::loadBindings(sol::state &lua, IApp &app) {

		sol::usertype<Renderer> renderer_t = lua.new_usertype<Renderer>("Renderer");

		renderer_t["visible"] = sol::property(
			&Renderer::getVisible,
			&Renderer::setVisible
		);

		renderer_t["color"] = sol::property(
			&Renderer::getColor,
			&Renderer::setColor
		);

		renderer_t["size"] = sol::property(
			&Renderer::getMeshSize,
			&Renderer::setMeshSize
		);

		renderer_t["shrink"] = sol::property(
			&Renderer::getMeshShrink,
			&Renderer::setMeshShrink
		);

		renderer_t["corner_visible"] = sol::property(
			&Renderer::getCornerVisible,
			&Renderer::setCornerVisible
		);

		renderer_t["attr_repeat"] = sol::property(
			&Renderer::getAttrRepeat,
			&Renderer::setAttrRepeat
		);

		renderer_t["light"] = sol::writeonly_property(&Renderer::setLight);


		// TODO important missing cases
		renderer_t.set_function("as", [](sol::this_state s, Renderer& self, const std::string& typeName) -> sol::object {
			if (typeName == "LineRenderer") {
				if (auto r = dynamic_cast<LineRenderer*>(&self)) {
					return sol::make_object(s, r);
				}
			} else if (typeName == "PointSetRenderer") {
				if (auto r = dynamic_cast<PointSetRenderer*>(&self)) {
					return sol::make_object(s, r);
				}
			}

			return sol::nil;
		});

		renderer_t.set_function("push", &Renderer::push);



		sol::usertype<PointSetRenderer> pointSetRenderer_t = lua.new_usertype<PointSetRenderer>(
			"PointSetRenderer",
			sol::base_classes, 
			sol::bases<Renderer>()
		);

		pointSetRenderer_t[sol::meta_function::index] = [](PointSetRenderer& self, int i) -> vec3 {
			return self[i - 1];
		};

		pointSetRenderer_t[sol::meta_function::new_index] = [](PointSetRenderer& self, int i, vec3 value) {
			self[i - 1] = value;
		};

		pointSetRenderer_t["visible"] = sol::property(
			&PointSetRenderer::getVisible,
			&PointSetRenderer::setVisible
		);
		pointSetRenderer_t["color"] = sol::property(
			&PointSetRenderer::getColor,
			&PointSetRenderer::setColor
		);
		pointSetRenderer_t["size"] = sol::property(
			&PointSetRenderer::getPointSize,
			&PointSetRenderer::setPointSize
		);
		pointSetRenderer_t.set_function("add_point", &PointSetRenderer::addPoint);
		pointSetRenderer_t.set_function("add_points", &PointSetRenderer::addPoints);
		pointSetRenderer_t.set_function("remove_points", &PointSetRenderer::removePoints);
		pointSetRenderer_t["count"] = sol::readonly_property(&PointSetRenderer::count);


		sol::usertype<HalfedgeRenderer> halfedgeRenderer_t = lua.new_usertype<HalfedgeRenderer>(
			"HalfedgeRenderer",
			sol::base_classes, 
			sol::bases<Renderer>()
		);

		halfedgeRenderer_t["visible"] = sol::property(
			&HalfedgeRenderer::getVisible,
			&HalfedgeRenderer::setVisible
		);
		halfedgeRenderer_t["inside_color"] = sol::property(
			&HalfedgeRenderer::getInsideColor,
			&HalfedgeRenderer::setInsideColor
		);
		halfedgeRenderer_t["outside_color"] = sol::property(
			&HalfedgeRenderer::getOutsideColor,
			&HalfedgeRenderer::setOutsideColor
		);
		halfedgeRenderer_t["thickness"] = sol::property(
			&HalfedgeRenderer::getThickness,
			&HalfedgeRenderer::setThickness
		);
		halfedgeRenderer_t["spacing"] = sol::property(
			&HalfedgeRenderer::getSpacing,
			&HalfedgeRenderer::setSpacing
		);
		halfedgeRenderer_t["padding"] = sol::property(
			&HalfedgeRenderer::getPadding,
			&HalfedgeRenderer::setPadding
		);


		auto line_t = lua.new_usertype<LineRenderer::Line>(
			"Line",
			sol::call_constructor, [](sol::table t) {
				LineRenderer::Line line;
				
				if (t["a"].valid() && t["a"].is<glm::vec3>())
					line.a = t["a"].get<glm::vec3>();
				if (t["b"].valid() && t["b"].is<glm::vec3>())
					line.b = t["b"].get<glm::vec3>();
				if (t["color"].valid() && t["color"].is<glm::vec3>())
					line.color = t["color"].get<glm::vec3>();
				
				return line;
			},
			"a", &LineRenderer::Line::a,
			"b", &LineRenderer::Line::b,
			"color", &LineRenderer::Line::color
		);

		auto lineRenderer_t = lua.new_usertype<LineRenderer>(
			"LineRenderer",
			sol::base_classes, 
			sol::bases<Renderer>(),
			"auto_update", sol::writeonly_property(&LineRenderer::setAutoUpdate)
		);

		lineRenderer_t.set_function("add_line", &LineRenderer::addLine);
		lineRenderer_t.set_function("add_lines", &LineRenderer::addLines);
		lineRenderer_t.set_function("clear_lines", &LineRenderer::clearLines);
		lineRenderer_t.set_function("push", &LineRenderer::push);

	}
}