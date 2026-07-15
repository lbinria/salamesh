#include "scene_model_bindings.h"

namespace bindings {

	void SceneModelBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<SceneModel>("SceneModel",
			"name", sol::readonly_property(&SceneModel::getName),
			"position", sol::property(&SceneModel::position, &SceneModel::position),
			"visible", sol::property(&SceneModel::isVisible, &SceneModel::setVisible),
			"mesh", sol::readonly_property(&SceneModel::getMesh),
			"add_shader", &SceneModel::addShaderPass,
			"materials", sol::readonly_property(&SceneModel::getMaterials),
			"get_material", &SceneModel::getMaterial,
			"get_material_params_group", &SceneModel::getMaterialParamsGroup,
			"set_layer", sol::resolve<void(Layer, ElementKind, bool)>(&SceneModel::setLayer),
			"set_layer", sol::resolve<void(Layer, ElementKind, const std::string, bool)>(&SceneModel::setLayer),
			"unset_layer", &SceneModel::unsetLayer,
			"unset_layers", &SceneModel::unsetLayers,
			"set_colormap", &SceneModel::setColormap,
			"get_colormap", &SceneModel::getColormap,
			"get_layer_attr", &SceneModel::getLayerAttr,
			"set_layer_attr", &SceneModel::setLayerAttr
		);
	}
}