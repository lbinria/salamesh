#pragma once 
#include "lua_binding.h"
#include "../core/app_interface.h"
#include "../core/scene.h"

namespace bindings {

	struct SceneBindings : LuaBinding {

		void loadBindings(sol::state &lua, IApp &app) override {

			type = lua.new_usertype<Scene>("Scene");

			type.set_function("load_model", &Scene::loadModel);
			type["models"] = sol::readonly_property(&Scene::getModels);
			type["model"] = sol::readonly_property(&Scene::getCurrentModel);
			type["hovered_model"] = sol::readonly_property(&Scene::getHoveredModel);
			type["selected_model"] = sol::property(&Scene::getSelectedModel, &Scene::setSelectedModel);
			type.set_function("focus", &Scene::focus);

			type["views"] = sol::readonly_property(&Scene::getViews);
			type["default_view"] = sol::readonly_property(&Scene::getDefaultView);
			type.set_function("get_view", &Scene::getView);
			
			type.set_function("add_view", &Scene::addView);
			type.set_function("remove_view", &Scene::removeView);

			type["default_render_surface"] = sol::readonly_property(&Scene::getDefaultRenderSurface);
			type["render_surfaces"] = sol::readonly_property(&Scene::getRenderSurfaces);

			type["camera"] = sol::readonly_property(&Scene::getCurrentCamera);
			type["cameras"] = sol::readonly_property(&Scene::getCameras);
			type["selected_camera"] = sol::property([](Scene &self) {
				return self.getSelectedCamera();
			}, [](Scene &self, std::string selected) {
				self.setSelectedCamera(selected);
			});

			type["renderers"] = sol::readonly_property(&Scene::getRenderers);

		}

		sol::usertype<Scene> type;
	};

}