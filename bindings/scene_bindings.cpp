#include "scene_bindings.h"

namespace bindings {

	void SceneBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<Scene>("Scene",
			// Model loading
			"load_model", &Scene::loadModel,
			
			// Model properties
			"models", sol::readonly_property(&Scene::getModels),
			"has_models", sol::readonly_property(&Scene::hasModels),
			"count_models", sol::readonly_property(&Scene::countModels),
			"current_model", sol::readonly_property(&Scene::getCurrentModel),
			"hovered_mesh", sol::readonly_property(&Scene::getHoveredMesh),
			"selected_model", sol::property(&Scene::getSelectedModel, &Scene::setSelectedModel),
			
			// Focus function
			"focus", &Scene::focus,
			
			"default_render_surface", sol::readonly_property(&Scene::getDefaultRenderSurface),

			// Camera properties
			"current_camera", sol::readonly_property(&Scene::getCurrentCamera),
			"cameras", sol::readonly_property(&Scene::getCameras),
			"selected_camera", sol::property(
				[](Scene &self) { return self.getSelectedCamera(); },
				[](Scene &self, std::string selected) { self.setSelectedCamera(selected); }
			),
			
			// Colormap functions
			"colormaps", sol::readonly_property(&Scene::getColormaps),
			"add_colormap", &Scene::addColormap,
			"remove_colormap", &Scene::removeColormap,
			"get_colormap", &Scene::getColormap
		);
	}
}