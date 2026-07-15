#include "scene_node_bindings.h"

namespace bindings {

	void SceneNodeBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<SceneNode>("SceneNode",
			"name", sol::readonly_property(&SceneNode::getName),
			"position", sol::property(&SceneNode::position, &SceneNode::position),
			"visible", sol::property(&SceneNode::isVisible, &SceneNode::setVisible),
			"mesh", sol::readonly_property(&SceneNode::getMesh),
			"add_shader", &SceneNode::addShaderPass,
			"materials", sol::readonly_property(&SceneNode::getMaterials),
			"get_material", &SceneNode::getMaterial,
			"get_material_params_group", &SceneNode::getMaterialParamsGroup,
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