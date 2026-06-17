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
			// "geometry", sol::property(&SceneNode::getGeometry, &SceneNode::setGeometry),
			"geometry", sol::readonly_property(&SceneNode::getGeometry),
			// "create_geometry", &SceneNode::createGeometry,
			"add_shader", &SceneNode::addShader,
			"has_shader", sol::readonly_property(&SceneNode::hasShader),
			"add_child", &SceneNode::add,
			"get_shader_buffers", sol::readonly_property(&SceneNode::getShaderBuffers),
			"get_shader_buffer", sol::overload(
				static_cast<std::optional<std::reference_wrapper<ShaderBuffer>>(SceneNode::*)(ShaderBase&)>(&SceneNode::getShaderBuffer),
				static_cast<std::optional<std::reference_wrapper<ShaderBuffer>>(SceneNode::*)(const std::string)>(&SceneNode::getShaderBuffer)
			),
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