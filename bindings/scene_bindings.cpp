#include "scene_bindings.h"

namespace bindings {

	void SceneBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<Scene>("Scene");

		type.set_function("load_model", &Scene::loadModel);
		type["nodes"] = sol::readonly_property(&Scene::getNodes);
		type["models"] = sol::readonly_property(&Scene::getNodes);
		
		type["has_nodes"] = sol::readonly_property(&Scene::hasNodes);
		type["count_nodes"] = sol::readonly_property(&Scene::countNodes);


		// type["model"] = sol::readonly_property(&Scene::getCurrentModel);
		type["model"] = sol::readonly_property(&Scene::getCurrentNode);
		type["hovered_model"] = sol::readonly_property(&Scene::getHoveredNode);
		type["selected_model"] = sol::property(&Scene::getSelectedNodeName, &Scene::setSelectedNode);
		// type.set_function("focus", &Scene::focus);
		type.set_function("focus", &Scene::focus);

		type["camera"] = sol::readonly_property(&Scene::getCurrentCamera);
		type["cameras"] = sol::readonly_property(&Scene::getCameras);
		type["selected_camera"] = sol::property([](Scene &self) {
			return self.getSelectedCamera();
		}, [](Scene &self, std::string selected) {
			self.setSelectedCamera(selected);
		});


		type["colormaps"] = sol::readonly_property(&Scene::getColormaps);
		type.set_function("add_colormap", &Scene::addColormap);
		type.set_function("remove_colormap", &Scene::removeColormap);
		type.set_function("get_colormap", &Scene::getColormap);

		// type["selected_colormap"], sol::property(
		// 	[](SceneNode &self) {
		// 		return self.getSelectedColormap() + 1;
		// 	}, 
		// 	[](SceneNode &self, int idx) {
		// 		self.setSelectedColormap(idx - 1);
		// 	}
		// )
	}
}