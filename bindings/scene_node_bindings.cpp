#include "scene_node_bindings.h"

namespace bindings {

	void SceneNodeBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<SceneNode>("SceneNode",
			"name", sol::readonly_property(&SceneNode::getName),
			"parent", sol::readonly_property(&SceneNode::getParent),
			"world_position", sol::readonly_property(&SceneNode::getWorldPosition),
			"position", sol::property(&SceneNode::position, &SceneNode::position),
			"bbox", sol::readonly_property([](SceneNode& m, sol::this_state s) {
				sol::state_view lua(s);
				auto [minv, maxv] = m.bbox();
				sol::table t = lua.create_table_with(1, minv, 2, maxv);
				return t;
			}),
			"visible", sol::property(&SceneNode::isVisible, &SceneNode::setVisible),
			"geometry", sol::readonly_property(&SceneNode::getGeometry),
			"add_shader", &SceneNode::addShader,
			"add_child", &SceneNode::addChild,
			"materials", sol::readonly_property(&SceneNode::getMaterials),
			"get_material", &SceneNode::getMaterial,
			"set_layer", sol::resolve<void(Layer, ElementKind, bool)>(&SceneNode::setLayer),
			"set_layer", sol::resolve<void(Layer, ElementKind, const std::string, bool)>(&SceneNode::setLayer),
			"unset_layer", &SceneNode::unsetLayer,
			"unset_layers", &SceneNode::unsetLayers,
			"set_colormap", &SceneNode::setColormap,
			"get_colormap", &SceneNode::getColormap,
			"get_layer_attr", &SceneNode::getLayerAttr,
			"set_layer_attr", &SceneNode::setLayerAttr
		);
	}
}