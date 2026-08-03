#include "scene_model_bindings.h"

namespace bindings {

	void SceneModelBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<SceneModel>("SceneModel",
			"name", sol::readonly_property(&SceneModel::getName),
			"position", sol::property(&SceneModel::position, &SceneModel::position),
			"visible", sol::property(&SceneModel::isVisible, &SceneModel::setVisible),
			"selected_attribute", sol::property(&SceneModel::getSelectedAttribute, &SceneModel::setSelectedAttribute),
			"mesh", sol::readonly_property(&SceneModel::getMesh),
			"add_shader", &SceneModel::addShaderPass,
			"materials", sol::readonly_property(&SceneModel::getMaterials),
			"get_material", &SceneModel::getMaterial,
			"get_material_params_group", &SceneModel::getMaterialParamsGroup,
			"set_layer", &SceneModel::setLayer,
			"set_layer_range", &SceneModel::setLayerRange,
			"unset_layer", &SceneModel::unsetLayer,
			"unset_layers", sol::resolve<void(bool)>(&SceneModel::unsetLayers),
			"unset_layers", sol::resolve<void(Layer, bool)>(&SceneModel::unsetLayers),
			"set_colormap", &SceneModel::setColormap,
			"get_colormap", &SceneModel::getColormap,
			"get_layer_attr", &SceneModel::getLayerAttr,
			"apply_material_from", &SceneModel::applyMaterialsFrom
		);
	}
}