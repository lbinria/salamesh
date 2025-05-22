#pragma once 
#define SOL_ALL_SAFETIES_ON 1
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

		imgui.set_function("InputText", [](const char* label, char* buf, size_t buf_size) {
			return ImGui::InputText(label, buf, buf_size);
		});

		// auto pf = imgui.set_function("Checkbox", [](const char* label, bool v) {
		// 	bool x = true;
		// 	ImGui::Checkbox(label, &x);
		// });

		auto pf = imgui.set_function("Checkbox", [](const char* label, sol::object v_lua, sol::this_state s) {
			sol::state_view lua(s);
			if (v_lua.is<bool>()) {
				bool v = v_lua.as<bool>();
				bool res = ImGui::Checkbox(label, &v);
				return sol::make_object(lua, std::make_tuple(v, res));
			} else {
				return sol::make_object(lua, sol::lua_nil);
			}
		});


		
		// auto pf = lua.set_function("Checkbox", [](const char* label, bool* v) -> bool {
		// 	if (v == nullptr) {
		// 		throw std::invalid_argument("Null pointer passed");
		// 	}
		// 	return ImGui::Checkbox(label, v);
		// });
		
		// pf.set_exception_handler([](lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description) -> int {
		// 	return sol::stack::push(L, "Error: " + std::string(maybe_exception.value().what()));

		// });

		imgui.set_function("SliderFloat", [](const char* label, float* v, float v_min, float v_max) {
			return ImGui::SliderFloat(label, v, v_min, v_max);
		});

		lua["imgui"] = imgui;

		// imgui.set_function("Checkbox", ImGui::Checkbox);

		// App bindings
		// auto app_tbl = lua.create_table();
		// lua["app"] = app_tbl;
		
		// app_tbl["test"] = sol::property(
		// 	[&app = app]() { return app.getTest(); },
		// 	[&app = app](int v) { app.setTest(v); }
		// );

		lua["app"] = &app;
		sol::usertype<IApp> app_tbl = lua.new_usertype<IApp>("IApp");
		app_tbl["test"] = sol::property(
			[&app = app]() { return app.getTest(); },
			[&app = app](float v) { app.setTest(v); }
		);

		app_tbl.set_function("print_test", [&app = app]() {
			app.printTest();
		});

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
		if (has_draw_gui) {
			auto call = draw_gui_func();
		}
	}

	private:

	sol::state lua;
	sol::function init_func;
	sol::function key_event_func;
	sol::function draw_gui_func;

	bool has_init, has_key_event, has_draw_gui;
};