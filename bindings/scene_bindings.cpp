#include "scene_bindings.h"

namespace bindings {

	void SceneBindings::loadBindings(sol::state &lua, IApp &app) {
		type = lua.new_usertype<Scene>("Scene");

		type.set_function("load_model", &Scene::loadModel);
		type["models"] = sol::readonly_property(&Scene::getModels);
		type["model"] = sol::readonly_property(&Scene::getCurrentModel);
		type["hovered_model"] = sol::readonly_property(&Scene::getHoveredModel);
		type["selected_model"] = sol::property(&Scene::getSelectedModel, &Scene::setSelectedModel);
		type.set_function("focus", &Scene::focus);

		type["camera"] = sol::readonly_property(&Scene::getCurrentCamera);
		type["cameras"] = sol::readonly_property(&Scene::getCameras);
		type["selected_camera"] = sol::property([](Scene &self) {
			return self.getSelectedCamera();
		}, [](Scene &self, std::string selected) {
			self.setSelectedCamera(selected);
		});

		type["renderers"] = sol::readonly_property(&Scene::getRenderers);

		type["colormaps"] = sol::readonly_property(&Scene::getColormaps);
		type.set_function("add_colormap", &Scene::addColormap);
		type.set_function("remove_colormap", &Scene::removeColormap);
		type.set_function("get_colormap", sol::overload(
			[](Scene &self, int idx) {
				return self.getColormap(idx);
			},
			[](Scene &self, const std::string name) {
				return self.getColormap(name);
			}
		));
	}
}