#pragma once 

#include <sol/sol.hpp>
#include "states.h"
// #include "app.h"
#include "app_interface.h"
#include <imgui.h>

// Declare app

struct LuaScript {

	LuaScript(IApp &app, const std::string& script) {

		load_bindings(app);

		// TODO load / bind symbols from C++ shared lib associated to the script

		try {
			lua.script_file(script);
		} catch (const sol::error& e) {
			std::cerr << "Error loading script: " << e.what() << std::endl;
		}

		// Get the function
		init_func = lua.get<sol::function>("init");
		key_event_func = lua.get<sol::function>("key_event");
		draw_gui_func = lua.get<sol::function>("draw_gui");

		// Check if function exists
		has_init = init_func.valid();
		has_key_event = key_event_func.valid();
		has_draw_gui = draw_gui_func.valid();

	}


	// TODO complete bindings
	// For a total control, I suggest to do imgui bindings (and others) "at hand":
	// - I look at SWIG quickly and it seems to be a bit heavy for what we need
	// - Don't want to create another tool to do that, that will parse code, etc. too heavy, too complicated
	// After doing it simply, we'll could think about doing better automated mecanisms
	void load_bindings(IApp &app) {

		lua.open_libraries(sol::lib::base);

		// Imgui bindings
		auto imgui = lua.create_table();
		lua["imgui"] = imgui;

		imgui.set_function("Begin", [](const char* name) {
			return ImGui::Begin(name);
		});

		imgui.set_function("End", []() {
			ImGui::End();
		});

		imgui.set_function("Text", [](const char* text) {
			ImGui::Text("%s", text);
		});

		imgui.set_function("Button", [](const char* label) {
			return ImGui::Button(label);
		});


		// App bindings
		auto app_tbl = lua.create_table();
		lua["app"] = app_tbl;
		
		app_tbl.set_function("reset_zoom", [&app = app]() {
			app.reset_zoom();
		});

		app_tbl.set_function("look_at_center", [&app = app]() {
			app.look_at_center();
		});

		// Camera bindings
		auto camera_tbl = lua.create_table();
		app_tbl["camera"] = camera_tbl;

		// camera_tbl.set_function("look_at", [&app = app](float x, float y, float z) {
		// 	app.camera->LookAt(glm::vec3(x, y, z));
		// });



	}

	inline void init() {
		if (has_init)
			init_func();
	}

	inline void key_event(int key, int scancode, int action, int mods)	{
		if (has_key_event)
			key_event_func(key, scancode, action, mods);
	}

	inline void draw_gui() {
		if (has_draw_gui)
			draw_gui_func();
	}

	private:

	sol::state lua;
	sol::function init_func;
	sol::function key_event_func;
	sol::function draw_gui_func;

	bool has_init, has_key_event, has_draw_gui;
};