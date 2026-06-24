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
			"hovered_node", sol::readonly_property(&Scene::getHoveredNode),
			"selected_node", sol::property(&Scene::getSelectedNode, &Scene::setSelectedNode),
			
			// Focus function
			"focus", &Scene::focus,
			
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

		// type = lua.new_usertype<Scene>("Scene");

		// type.set_function("load_model", &Scene::loadModel);
		// type["nodes"] = sol::readonly_property(&Scene::getNodes);
		
		// type["has_nodes"] = sol::readonly_property(&Scene::hasNodes);
		// type["count_nodes"] = sol::readonly_property(&Scene::countNodes);

		// type["current_node"] = sol::readonly_property(&Scene::getCurrentNode);
		// type["hovered_node"] = sol::readonly_property(&Scene::getHoveredNode);
		// type["selected_node"] = sol::property(&Scene::getSelectedNode, &Scene::setSelectedNode);
		// // type.set_function("focus", &Scene::focus);
		// type.set_function("focus", &Scene::focus);

		// type["camera"] = sol::readonly_property(&Scene::getCurrentCamera);
		// type["cameras"] = sol::readonly_property(&Scene::getCameras);
		// type["selected_camera"] = sol::property([](Scene &self) {
		// 	return self.getSelectedCamera();
		// }, [](Scene &self, std::string selected) {
		// 	self.setSelectedCamera(selected);
		// });


		// type["colormaps"] = sol::readonly_property(&Scene::getColormaps);
		// type.set_function("add_colormap", &Scene::addColormap);
		// type.set_function("remove_colormap", &Scene::removeColormap);
		// type.set_function("get_colormap", &Scene::getColormap);
	}
}