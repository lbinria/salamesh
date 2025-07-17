#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"

namespace bindings {

	struct AppBindings {

		static sol::usertype<IApp> loadBindings(sol::state &lua, IApp &app) {

			// Input state binding
			sol::usertype<InputState::PrimitiveState> primitive_state_t = lua.new_usertype<InputState::PrimitiveState>("PrimitiveState", 
				sol::constructors<InputState::PrimitiveState()>(),
				"hovered", sol::property(
					&InputState::PrimitiveState::get_hovered,
					&InputState::PrimitiveState::set_hovered
				),
				"is_hovered", sol::readonly_property(&InputState::PrimitiveState::is_hovered),
				"is_changed", sol::readonly_property(&InputState::PrimitiveState::is_changed)
			);

			sol::usertype<InputState> input_state_t = lua.new_usertype<InputState>("InputState", 
				sol::constructors<InputState(), InputState::PrimitiveState()>(),
				"vertex", sol::readonly_property(&InputState::vertex),
				"edge", sol::readonly_property(&InputState::edge),
				"facet", sol::readonly_property(&InputState::facet),
				"cell", sol::readonly_property(&InputState::cell)
			);

			// App bindings
			lua["app"] = &app;
			sol::usertype<IApp> app_type = lua.new_usertype<IApp>("IApp");

			// TODO maybe move 2 lines to app bindings
			app_type["models"] = sol::readonly_property(&IApp::getModels);
			app_type["current_model"] = sol::readonly_property(&IApp::getCurrentModel);
			app_type["cameras"] = sol::readonly_property(&IApp::getCameras);
			app_type["camera"] = sol::readonly_property(&IApp::getCamera);
			
			app_type["selected_model"] = sol::property([](IApp &self) {
				return self.getSelectedModel() + 1;
			}, [](IApp &self, int selected) {
				self.setSelectedModel(selected);
			});

			app_type["selected_camera"] = sol::property([](IApp &self) {
				return self.getSelectedCamera() + 1;
			}, [](IApp &self, int selected) {
				self.setSelectedCamera(selected - 1);
			});

			app_type["input_state"] = sol::readonly_property(&IApp::getInputState);

			app_type["pick_mode_strings"] = sol::readonly_property(&IApp::getPickModeStrings);

			app_type["pick_mode"] = sol::property(
				&IApp::getPickMode,
				&IApp::setPickMode
			);

			app_type["colormaps_2d"] = sol::readonly_property(&IApp::getColorMaps2D);

			app_type.set_function("load_model", &IApp::load_model);
			
			app_type.set_function("screenshot", [&app = app](const std::string& filename) {
				app.screenshot(filename);
			});

			app_type.set_function("setCullMode", [&app = app](int mode) {
				app.setCullMode(mode);
			});

			return app_type;
		}

	};

}