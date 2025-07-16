#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <optional>

#include "core/component.h"
#include "core/states.h"
// #include "app.h"
#include "core/app_interface.h"
#include <imgui.h>

#include "bindings/imgui_bindings.h"
#include "bindings/glm_bindings.h"
#include "bindings/camera_bindings.h"
#include "bindings/model_bindings.h"


// Declare app

struct LuaScript : public Component {

	LuaScript(IApp &app, const std::string& script) {

		load_bindings(app);

		// TODO load / bind symbols from C++ shared lib associated to the script

		try {
			// lua.script_file(script);
			lua.safe_script_file(script);
		} catch (const sol::error& e) {
			std::cerr << "Error loading script: " << e.what() << std::endl;
		}

		// Get the function
		init_func = lua.get<sol::protected_function>("init");
		draw_gui_func = lua.get<sol::protected_function>("draw_gui");

		mouse_move_func = lua.get<sol::protected_function>("mouse_move");
		mouse_button_func = lua.get<sol::protected_function>("mouse_button");
		mouse_scroll_func = lua.get<sol::protected_function>("mouse_scroll");
		key_event_func = lua.get<sol::protected_function>("key_event");

		// Check if function exists
		has_init = init_func.valid();
		has_draw_gui = draw_gui_func.valid();

		has_mouse_button = mouse_button_func.valid();
		has_mouse_move = mouse_move_func.valid();
		has_mouse_scroll = mouse_scroll_func.valid();
		has_key_event = key_event_func.valid();
	}


	// TODO complete bindings
	// For a total control, I suggest to do imgui bindings (and others) "at hand":
	// - I look at SWIG quickly and it seems to be a bit heavy for what we need
	// - Don't want to create another tool to do that, that will parse code, etc. too heavy, too complicated
	// After doing it simply, we'll could think about doing better automated mecanisms
	void load_bindings(IApp &app) {

		lua.open_libraries(sol::lib::base);
		lua.open_libraries(sol::lib::string);
		lua.open_libraries(sol::lib::math);
		lua.open_libraries(sol::lib::os);

		bindings::ImGuiBindings::load_bindings(lua);


		// App bindings
		lua["app"] = &app;
		sol::usertype<IApp> app_type = lua.new_usertype<IApp>("IApp");

		
		app_type["selected_model"] = sol::property([](IApp &self) {
			return self.getSelectedModel() + 1;
		}, [](IApp &self, int selected) {
			self.setSelectedModel(selected);
		});


		bindings::GlmBindings::load_bindings(lua, app);
		bindings::CameraBindings::load_bindings(lua, app_type, app);
		bindings::ModelBindings::load_bindings(lua, app_type, app);


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






	}

	void init() final override {
		if (has_init)
			init_func();
	}

	void setup() final override {}
	void cleanup() final override {}

	void mouse_move(double x, double y) {
		if (has_mouse_move)
			mouse_move_func(x, y);
	}

	void mouse_button(int button, int action, int mods) {
		if (has_mouse_button)
			mouse_button_func(button, action, mods);
	}

	void mouse_scroll(double xoffset, double yoffset) {
		if (has_mouse_scroll)
			mouse_scroll_func(xoffset, yoffset);
	}

	inline void key_event(int key, int scancode, int action, int mods) final override {
		if (has_key_event)
			key_event_func(key, scancode, action, mods);
	}

	inline bool draw_gui(ImGuiContext *ctx) final override {
		if (!has_draw_gui)
			return true;


		try {
			auto result = draw_gui_func();

			if (!result.valid()) {
				sol::error err = result;
				std::cerr << "Error:" << err.what() << ". Note that this may cause the app to crash." << std::endl;
				status = SCRIPT_STATUS_FAILED;
				return false;
			}
		} catch (const sol::error &err) {
			std::cerr << "Error: " << err.what() << std::endl;
			return false;
		}
		
		return true;
	}

	enum Status {
		SCRIPT_STATUS_OK,
		SCRIPT_STATUS_FAILED
	};

	Status status = SCRIPT_STATUS_OK;

	private:

	sol::state lua;
	sol::protected_function init_func;
	sol::protected_function mouse_move_func;
	sol::protected_function mouse_button_func;
	sol::protected_function mouse_scroll_func;
	sol::protected_function key_event_func;
	sol::protected_function draw_gui_func;

	bool has_init, has_mouse_move, has_mouse_button, has_mouse_scroll, has_key_event, has_draw_gui;


};