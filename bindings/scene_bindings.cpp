#include "scene_bindings.h"

namespace bindings {

	void SceneBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<Scene>("Scene",
			// Model loading
			"load_model", &Scene::loadModel,
			
			// Node properties
			"nodes", sol::readonly_property(&Scene::getNodes),
			"has_nodes", sol::readonly_property(&Scene::hasNodes),
			"count_nodes", sol::readonly_property(&Scene::countNodes),
			"current_node", sol::readonly_property(&Scene::getCurrentNode),
			"hovered_mesh", sol::readonly_property(&Scene::getHoveredMesh),
			"selected_node", sol::property(&Scene::getSelectedNode, &Scene::setSelectedNode),
			
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