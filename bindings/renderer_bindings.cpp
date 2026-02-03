#include "renderer_bindings.h"

#include "../core/renderers/renderer.h"
#include "../core/renderers/point_set_renderer.h"
#include "../core/renderers/halfedge_renderer.h"
#include "../core/renderers/line_renderer.h"

namespace bindings {

	void RendererBindings::loadBindings(sol::state &lua, IApp &app) {

		sol::usertype<IRenderer> renderer_t = lua.new_usertype<IRenderer>("IRenderer");

		renderer_t["visible"] = sol::property(
			&IRenderer::getVisible,
			&IRenderer::setVisible
		);

		renderer_t["color"] = sol::property(
			&IRenderer::getColor,
			&IRenderer::setColor
		);

		renderer_t["size"] = sol::property(
			&IRenderer::getMeshSize,
			&IRenderer::setMeshSize
		);

		renderer_t["shrink"] = sol::property(
			&IRenderer::getMeshShrink,
			&IRenderer::setMeshShrink
		);

		renderer_t["corner_visible"] = sol::property(
			&IRenderer::getCornerVisible,
			&IRenderer::setCornerVisible
		);

		renderer_t["attr_repeat"] = sol::property(
			&IRenderer::getAttrRepeat,
			&IRenderer::setAttrRepeat
		);

		// TODO important missing cases
		renderer_t.set_function("as", [](sol::this_state s, IRenderer& self, const std::string& typeName) -> sol::object {
			if (typeName == "LineRenderer") {
				if (auto lineRenderer = dynamic_cast<LineRenderer*>(&self)) {
					return sol::make_object(s, lineRenderer);
				}
			}

			return sol::nil;
		});


		sol::usertype<PointSetRenderer> pointSetRenderer_t = lua.new_usertype<PointSetRenderer>(
			"PointSetRenderer",
			sol::base_classes, 
			sol::bases<IRenderer>()
		);

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

		sol::usertype<HalfedgeRenderer> halfedgeRenderer_t = lua.new_usertype<HalfedgeRenderer>(
			"HalfedgeRenderer",
			sol::base_classes, 
			sol::bases<IRenderer>()
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
			sol::bases<IRenderer>(),
			"auto_update", sol::writeonly_property(&LineRenderer::setAutoUpdate)
		);

		lineRenderer_t.set_function("add_line", &LineRenderer::addLine);
		lineRenderer_t.set_function("clear_lines", &LineRenderer::clearLines);
		lineRenderer_t.set_function("push", &LineRenderer::push);

	}
}