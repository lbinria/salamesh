#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"
#include "../core/element.h"

namespace bindings {

	struct AppBindings {

		static sol::usertype<IApp> loadBindings(sol::state &lua, IApp &app) {

			// Input state binding
			sol::usertype<InputState::PrimitiveState> primitive_state_t = lua.new_usertype<InputState::PrimitiveState>("PrimitiveState", 
				sol::constructors<InputState::PrimitiveState()>(),
				"hovered", sol::property(
					&InputState::PrimitiveState::getHovered,
					&InputState::PrimitiveState::setHovered
				),
				"all_hovered", sol::readonly_property(&InputState::PrimitiveState::getAllHovered),
				"any_hovered", sol::readonly_property(&InputState::PrimitiveState::anyHovered),
				"has_changed", sol::readonly_property(&InputState::PrimitiveState::hasChanged)
			);

			sol::usertype<InputState> input_state_t = lua.new_usertype<InputState>("InputState", 
				sol::constructors<InputState(), InputState::PrimitiveState()>(),
				"vertex", sol::readonly_property(&InputState::vertex),
				"edge", sol::readonly_property(&InputState::edge),
				"facet", sol::readonly_property(&InputState::facet),
				"cell", sol::readonly_property(&InputState::cell),
				"mesh", sol::readonly_property(&InputState::mesh)
			);

			// General functions 
			lua.set_function("elementKindToString", &elementKindToString);
			lua.set_function("elementTypeToString", &elementTypeToString);

			// App bindings
			lua["app"] = &app;
			sol::usertype<IApp> app_type = lua.new_usertype<IApp>("IApp");

			app_type["count_models"] = sol::readonly_property(&IApp::countModels);
			app_type["has_models"] = sol::readonly_property(&IApp::hasModels);
			app_type["models"] = sol::readonly_property(&IApp::getModels);
			app_type.set_function("getChildrenOf", &IApp::getChildrenOf);
			app_type["model"] = sol::readonly_property(&IApp::getCurrentModel);

			app_type["cameras"] = sol::readonly_property(&IApp::getCameras);
			app_type["camera"] = sol::readonly_property(&IApp::getCamera);
			
			app_type["selected_model"] = sol::property([](IApp &self) {
				return self.getSelectedModel() + 1;
			}, [](IApp &self, int selected) {
				self.setSelectedModel(selected - 1);
			});

			app_type["selected_camera"] = sol::property([](IApp &self) {
				return self.getSelectedCamera() + 1;
			}, [](IApp &self, int selected) {
				self.setSelectedCamera(selected - 1);
			});

			app_type["input_state"] = sol::readonly_property(&IApp::getInputState);

			app_type["pick_mode_strings"] = sol::readonly_property(&IApp::getPickModeStrings);

			// app_type["pick_mode"] = sol::property(
			// 	&IApp::getPickMode,
			// 	&IApp::setPickMode
			// );

			app_type["colormaps_2d"] = sol::readonly_property(&IApp::getColorMaps2D);

			app_type.set_function("load_model", &IApp::loadModel);
			
			app_type.set_function("add_model", [](IApp &self, std::string name) {
				return self.addModel(name) + 1;
			});

			app_type.set_function("remove_model", sol::overload(
				[](IApp &self, int idx) {
					self.removeModel(idx);
				},
				[](IApp &self, std::string name) -> bool {
					return self.removeModel(name);
				}
			));

			app_type.set_function("get_model_by_name", &IApp::getModelByName);

			app_type.set_function("get_index_of_model", [](IApp &self, std::string name) {
				int idx = self.getIndexOfModel(name);
				return idx != -1 ? idx + 1 : -1;
			});
			
			app_type.set_function("screenshot", [&app = app](const std::string& filename) {
				app.screenshot(filename);
			});

			app_type.set_function("setCullMode", [&app = app](int mode) {
				app.setCullMode(mode);
			});

			app_type["cull_mode"] = sol::writeonly_property(&IApp::setCullMode);
			app_type["cull"] = sol::property(&IApp::getCull, &IApp::setCull);

			return app_type;
		}

	};

}