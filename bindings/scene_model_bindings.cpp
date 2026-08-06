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
			"layers", sol::readonly_property(&SceneModel::layers),
			"get_material", &SceneModel::getMaterial,
			"get_material_params_group", &SceneModel::getMaterialParamsGroup,
			"set_colormap", &SceneModel::setColormap,
			"get_colormap", &SceneModel::getColormap,
			"apply_material_from", &SceneModel::applyMaterialsFrom
		);
	}
}