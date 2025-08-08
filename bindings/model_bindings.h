#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"
#include "../core/model.h"
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
				"ptr", sol::readonly_property(&Attribute::getPtr),
				"double_container", sol::readonly_property(&Attribute::getContainer<double>),
				"int_container", sol::readonly_property(&Attribute::getContainer<int>),
				"bool_container", sol::readonly_property(&Attribute::getContainer<bool>)
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

			model_t["color"] = sol::property(
				&Model::getColor,
				&Model::setColor
			);

			model_t["point_visible"] = sol::property(
				&Model::getPointVisible,
				&Model::setPointVisible
			);

			model_t["position"] = sol::property(
				&Model::getPosition,
				&Model::setPosition
			);

			model_t["world_position"] = sol::readonly_property(
				&Model::getWorldPosition
			);

			model_t["light"] = sol::property(
				&Model::getLight,
				&Model::setLight
			);

			// model_t.set_function("getLight", &Model::getLight);
			// model_t.set_function("setLight", &Model::setLight);

			model_t["is_light_follow_view"] = sol::property(
				&Model::getLightFollowView,
				&Model::setLightFollowView
			);

			model_t["clipping"] = sol::property(
				&Model::getClipping,
				&Model::setClipping
			);

			// model_t.set_function("getClipping", &Model::getClipping);
			// model_t.set_function("setClipping", &Model::setClipping);

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

			model_t["meshSize"] = sol::property(
				&Model::getMeshSize,
				&Model::setMeshSize
			);

			model_t["meshShrink"] = sol::property(
				&Model::getMeshShrink,
				&Model::setMeshShrink
			);

			model_t["point_color"] = sol::property(
				&Model::getPointColor,
				&Model::setPointColor
			);

			model_t["point_size"] = sol::property(
				&Model::getPointSize,
				&Model::setPointSize
			);

			model_t["edge_size"] = sol::property(
				&Model::getEdgeSize,
				&Model::setEdgeSize
			);

			model_t["edge_inside_color"] = sol::property(
				&Model::getEdgeInsideColor,
				&Model::setEdgeInsideColor
			);

			model_t["edge_outside_color"] = sol::property(
				&Model::getEdgeOutsideColor,
				&Model::setEdgeOutsideColor
			);

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

			model_t["selected_colormap"] = sol::property(
				&Model::getSelectedColormap,
				&Model::setSelectedColormap
			);


		}

	};

}