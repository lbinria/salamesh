#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <optional>
#include <imgui.h>

#include "core/app_interface.h"
#include "core/component.h"
#include "core/states.h"

#include "bindings/imgui_bindings.h"
#include "bindings/app_bindings.h"
#include "bindings/glm_bindings.h"
#include "bindings/camera_bindings.h"
#include "bindings/model_bindings.h"

struct LuaScript final : public Component {

	LuaScript(IApp &app, const std::string& script) {

		loadBindings(app);

		// TODO load / bind symbols from C++ shared lib associated to the script

		try {
			// lua.script_file(script);
			lua.safe_script_file(script);
		} catch (const sol::error& e) {
			std::cerr << "Error loading script: " << e.what() << std::endl;
		}

		// Get the functions
		init_func = lua.get<sol::protected_function>("init");
		draw_gui_func = lua.get<sol::protected_function>("draw_gui");
		update_func = lua.get<sol::protected_function>("update");
		cleanup_func = lua.get<sol::protected_function>("cleanup");

		mouse_move_func = lua.get<sol::protected_function>("mouse_move");
		mouse_button_func = lua.get<sol::protected_function>("mouse_button");
		mouse_scroll_func = lua.get<sol::protected_function>("mouse_scroll");
		key_event_func = lua.get<sol::protected_function>("key_event");

		// Check whether functions exists
		has_init = init_func.valid();
		has_draw_gui = draw_gui_func.valid();
		has_update = update_func.valid();
		has_cleanup = cleanup_func.valid();

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
	void loadBindings(IApp &app) {

		lua.open_libraries(sol::lib::base);
		lua.open_libraries(sol::lib::string);
		lua.open_libraries(sol::lib::math);
		lua.open_libraries(sol::lib::os);

		bindings::ImGuiBindings::loadBindings(lua);
		auto app_type = bindings::AppBindings::loadBindings(lua, app);
		bindings::GlmBindings::loadBindings(lua, app);
		bindings::CameraBindings::loadBindings(lua, app_type, app);
		bindings::ModelBindings::loadBindings(lua, app_type, app);
	}

	// Lifecycle

	void init() override {
		if (has_init)
			init_func();
	}

	bool draw_gui(ImGuiContext *ctx) override {
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

	void update(float dt) override {
		if (has_update)
			update_func(dt);
	}

	void cleanup() override {
		if (has_cleanup)
			cleanup_func();
	}

	// Events

	void mouse_move(double x, double y) override {
		if (has_mouse_move)
			mouse_move_func(x, y);
	}

	void mouse_button(int button, int action, int mods) override {
		if (has_mouse_button)
			mouse_button_func(button, action, mods);
	}

	void mouse_scroll(double xoffset, double yoffset) override {
		if (has_mouse_scroll)
			mouse_scroll_func(xoffset, yoffset);
	}

	void key_event(int key, int scancode, int action, int mods) override {
		if (has_key_event)
			key_event_func(key, scancode, action, mods);
	}

	void componentChanged(const std::string &id) override {
		// Not implemented
	}


	enum Status {
		SCRIPT_STATUS_OK,
		SCRIPT_STATUS_FAILED
	};

	Status status = SCRIPT_STATUS_OK;

	private:

	sol::state lua;
	sol::protected_function init_func;
	sol::protected_function draw_gui_func;
	sol::protected_function update_func;
	sol::protected_function cleanup_func;

	sol::protected_function mouse_move_func;
	sol::protected_function mouse_button_func;
	sol::protected_function mouse_scroll_func;
	sol::protected_function key_event_func;


	bool has_init, has_mouse_move, has_mouse_button, has_mouse_scroll, has_key_event, has_draw_gui, has_update, has_cleanup;

};