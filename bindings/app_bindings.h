#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include "../core/app_interface.h"

namespace bindings {

	struct AppBindings {

		static sol::usertype<IApp> loadBindings(sol::state &lua, IApp &app) {

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
			// app_type["selected_camera"] = sol::property(
			// 	&IApp::getSelectedCamera,
			// 	&IApp::setSelectedCamera
			// );

			app_type["pick_mode_strings"] = sol::readonly_property(
				&IApp::getPickModeStrings
			);

			app_type["pick_mode"] = sol::property(
				&IApp::getPickMode,
				&IApp::setPickMode
			);

			app_type["colormaps_2d"] = sol::readonly_property(
				&IApp::getColorMaps2D
			);

			app_type.set_function("load_model", &IApp::load_model);
			// app_type.set_function("screenshot", &IApp::screenshot);
			// app_type.set_function("load_model", [&app = app](const std::string& filename) {
			// 	app.load_model(filename);
			// });
			
			app_type.set_function("screenshot", [&app = app](const std::string& filename) {
				app.screenshot(filename);
			});

			app_type.set_function("setClipping", [&app = app](bool b) {
				app.setClipping(b);
			});

			app_type.set_function("setCullMode", [&app = app](int mode) {
				app.setCullMode(mode);
			});

			return app_type;
		}

	};

}