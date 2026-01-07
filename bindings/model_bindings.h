#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"
#include "../core/models/model.h"
#include "../core/attribute.h"

namespace bindings {

	struct ModelBindings {

		static void loadBindings(sol::state &lua, sol::usertype<IApp>& app_type, IApp &app) {

			// Create container user types for different attribute types
			sol::usertype<Attribute::Container<double>> attr_double_container_t = lua.new_usertype<Attribute::Container<double>>("AttributeContainerDouble");

			attr_double_container_t[sol::meta_function::index] = [](Attribute::Container<double>& c, int i) {
				return c[i];
			};

			attr_double_container_t[sol::meta_function::new_index] = [](Attribute::Container<double>& c, int i, double value) {
				c[i] = value;
			};

			sol::usertype<Attribute::Container<int>> attr_int_container_t = lua.new_usertype<Attribute::Container<int>>("AttributeContainerInt");

			attr_int_container_t[sol::meta_function::index] = [](Attribute::Container<int>& c, int i) {
				return c[i];
			};

			attr_int_container_t[sol::meta_function::new_index] = [](Attribute::Container<int>& c, int i, int value) {
				c[i] = value;
			};

			// TODO complete to access attribute data
			// sol::usertype<Attribute::Container<bool>> attr_bool_container_t = lua.new_usertype<Attribute::Container<bool>>("AttributeContainerBool");
			
			// attr_bool_container_t[sol::meta_function::index] = [](Attribute::Container<bool>& c, int i) {
			// 	return c[i];
			// };

			// attr_bool_container_t[sol::meta_function::new_index] = [](Attribute::Container<bool>& c, int i, bool value) {
			// 	c[i] = value;
			// };

			sol::usertype<Attribute> attr_t = lua.new_usertype<Attribute>("Attribute", 
				sol::constructors<Attribute()>(),
				"name", sol::readonly_property(&Attribute::getName),
				"kind", sol::readonly_property(&Attribute::getKind),
				"type", sol::readonly_property(&Attribute::getType),
				"dim", sol::readonly_property(&Attribute::getDims),
				"ptr", sol::readonly_property(&Attribute::getPtr),
				"double_container", sol::readonly_property(&Attribute::getContainer<double>),
				"int_container", sol::readonly_property(&Attribute::getContainer<int>),
				"bool_container", sol::readonly_property(&Attribute::getContainer<bool>),
				"vec2_container", sol::readonly_property(&Attribute::getContainer<vec2>),
				"vec3_container", sol::readonly_property(&Attribute::getContainer<vec3>)
			);


			sol::usertype<PointSetRenderer> pointSetRenderer_t = lua.new_usertype<PointSetRenderer>("PointSetRenderer");

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

			sol::usertype<HalfedgeRenderer> halfedgeRenderer_t = lua.new_usertype<HalfedgeRenderer>("HalfedgeRenderer");

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

			sol::usertype<IRenderer> meshRenderer_t = lua.new_usertype<IRenderer>("MeshRenderer");

			meshRenderer_t["visible"] = sol::property(
				&IRenderer::getVisible,
				&IRenderer::setVisible
			);

			meshRenderer_t["color"] = sol::property(
				&IRenderer::getColor,
				&IRenderer::setColor
			);

			meshRenderer_t["size"] = sol::property(
				&IRenderer::getMeshSize,
				&IRenderer::setMeshSize
			);

			meshRenderer_t["shrink"] = sol::property(
				&IRenderer::getMeshShrink,
				&IRenderer::setMeshShrink
			);

			meshRenderer_t["corner_visible"] = sol::property(
				&IRenderer::getCornerVisible,
				&IRenderer::setCornerVisible
			);

			meshRenderer_t["attr_repeat"] = sol::property(
				&IRenderer::getAttrRepeat,
				&IRenderer::setAttrRepeat
			);

			sol::usertype<Model> model_t = lua.new_usertype<Model>("Model");

			model_t.set_function("load", &Model::load);
			model_t.set_function("save", &Model::save);
			model_t.set_function("save_as", &Model::saveAs);

			model_t["name"] = sol::property(
				&Model::getName,
				&Model::setName
			);

			model_t["parent"] = sol::property(
				&Model::getParent,
				&Model::setParent
			);

			model_t["visible"] = sol::property(
				&Model::getVisible,
				&Model::setVisible
			);

			model_t["position"] = sol::property(
				&Model::getPosition,
				&Model::setPosition
			);

			model_t["world_position"] = sol::readonly_property(
				&Model::getWorldPosition
			);

			model_t["bbox"] = sol::readonly_property([](Model& m, sol::this_state s) {
				sol::state_view lua(s);
				auto [minv, maxv] = m.bbox();
				sol::table t = lua.create_table_with(1, minv, 2, maxv);
				return t;
			});

			model_t["center"] = sol::readonly_property(
				&Model::getCenter
			);

			model_t["radius"] = sol::readonly_property(
				&Model::getRadius
			);

			model_t["nverts"] = sol::readonly_property(
				&Model::nverts
			);

			model_t["nfacets"] = sol::readonly_property(
				&Model::nfacets
			);

			model_t["ncorners"] = sol::readonly_property(
				&Model::ncorners
			);

			model_t["nhalfedges"] = sol::readonly_property(
				&Model::nhalfedges
			);

			model_t["ncells"] = sol::readonly_property(
				&Model::ncells
			);


			model_t["light"] = sol::property(
				&Model::getLight,
				&Model::setLight
			);

			model_t["is_light_follow_view"] = sol::property(
				&Model::getLightFollowView,
				&Model::setLightFollowView
			);

			model_t["clipping_mode"] = sol::property([](Model &self) {
				return self.getClippingMode() + 1;
			}, [](Model &self, int selected) {
				self.setClippingMode(static_cast<IRenderer::ClippingMode>(selected - 1));
			});

			model_t["clipping"] = sol::property(
				&Model::getClipping,
				&Model::setClipping
			);

			model_t["clipping_plane_point"] = sol::property(
				&Model::getClippingPlanePoint,
				&Model::setClippingPlanePoint
			);

			model_t["clipping_plane_normal"] = sol::property(
				&Model::getClippingPlaneNormal,
				&Model::setClippingPlaneNormal
			);

			model_t["invert_clipping"] = sol::property(
				&Model::getInvertClipping,
				&Model::setInvertClipping
			);

			model_t.set_function("setup_clipping", &Model::setupClipping);

			model_t["clipping_mode_strings"] = sol::readonly_property(&Model::getClippingModeStrings);

			// Renderers accesses
			model_t["points"] = sol::readonly_property(&Model::getPoints);
			model_t["edges"] = sol::readonly_property(&Model::getEdges);
			model_t["mesh"] = sol::readonly_property(&Model::getMesh);


			// TODO here change implementation by managing index offset from lua to C++ +1 / -1
			model_t["color_mode_strings"] = sol::readonly_property(
				&Model::getColorModeStrings
			);

			model_t["color_mode"] = sol::property(
				&Model::getColorMode,
				&Model::setColorMode
			);

			model_t["attrs"] = sol::readonly_property([&lua = lua](Model &self) {
				sol::table attrs_tbl = lua.create_table();
				for (auto &attr : self.getAttrs()) {
					sol::table item = lua.create_table();
					attrs_tbl.add(attr);
				}
				return attrs_tbl;
			});

			model_t["selected_attr"] = sol::property([](Model &self) {
				return self.getSelectedAttr() + 1;
			}, [](Model &self, int selected) {
				self.setSelectedAttr(selected - 1);
			});

			model_t["selected_attr0"] = sol::property([](Model &self) {
				return self.getSelectedAttr0() + 1;
			}, [](Model &self, int selected) {
				self.setSelectedAttr0(selected - 1);
			});

			model_t["selected_attr1"] = sol::property([](Model &self) {
				return self.getSelectedAttr1() + 1;
			}, [](Model &self, int selected) {
				self.setSelectedAttr1(selected - 1);
			});

			model_t["selected_attr2"] = sol::property([](Model &self) {
				return self.getSelectedAttr2() + 1;
			}, [](Model &self, int selected) {
				self.setSelectedAttr2(selected - 1);
			});


			
			model_t.set_function("set_colormap0_attr", &Model::setColormap0Attr);
			model_t.set_function("set_colormap1_attr", &Model::setColormap1Attr);
			model_t.set_function("set_colormap2_attr", &Model::setColormap2Attr);

			model_t.set_function("set_colormap0", &Model::setColormap0);
			model_t.set_function("set_colormap1", &Model::setColormap1);
			model_t.set_function("set_colormap2", &Model::setColormap2);
			
			model_t.set_function("unset_colormap0", &Model::unsetColormap0);
			model_t.set_function("unset_colormap1", &Model::unsetColormap1);
			model_t.set_function("unset_colormap2", &Model::unsetColormap2);
			

			// TODO here change implementation by managing index offset from lua to C++ +1 / -1
			model_t["selected_colormap"] = sol::property(
				&Model::getSelectedColormap,
				&Model::setSelectedColormap
			);

			model_t["selected_colormap0"] = sol::property(
				&Model::getSelectedColormap0,
				&Model::setSelectedColormap0
			);

			model_t["selected_colormap1"] = sol::property(
				&Model::getSelectedColormap1,
				&Model::setSelectedColormap1
			);

			model_t["selected_colormap2"] = sol::property(
				&Model::getSelectedColormap2,
				&Model::setSelectedColormap2
			);



		}

	};

}