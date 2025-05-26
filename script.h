#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <optional>

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
			// lua.script_file(script);
			lua.safe_script_file(script);
		} catch (const sol::error& e) {
			std::cerr << "Error loading script: " << e.what() << std::endl;
		}

		// Get the function
		init_func = lua.get<sol::protected_function>("init");
		key_event_func = lua.get<sol::protected_function>("key_event");
		draw_gui_func = lua.get<sol::protected_function>("draw_gui");

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

		auto pf = imgui.set_function("Checkbox", [](const char* label, sol::object v, sol::this_state s) -> std::optional<std::tuple<bool, bool>> {
			sol::state_view lua(s);

			if (v.is<bool>()) {
				bool val = v.as<bool>();
				bool sel = ImGui::Checkbox(label, &val);
				return std::make_optional(std::make_tuple(sel, val));

			} else {
				return std::nullopt;
			}

		});

		imgui.set_function("SliderFloat", [](const char* label, sol::object v, float v_min, float v_max, sol::this_state s) -> std::optional<std::tuple<bool, float>> {
			sol::state_view lua(s);

			if (v.is<float>()) {
				float val = v.as<float>();
				bool sel = ImGui::SliderFloat(label, &val, v_min, v_max);
				return std::make_optional(std::make_tuple(sel, val));
			} else {
				return std::nullopt;
			}
		});

		lua["imgui"] = imgui;


		// App bindings
		lua["app"] = &app;
		sol::usertype<IApp> app_type = lua.new_usertype<IApp>("IApp");
		// Camera bindings
		auto camera_tbl = lua.create_table();
		app_type["camera"] = camera_tbl;
		// Renderer bindings
		auto renderer_tbl = lua.create_table();
		app_type["renderer"] = renderer_tbl;

		camera_tbl.set_function("LookAt", [&app = app](float x, float y, float z) {
			app.getCamera().LookAt(glm::vec3(x, y, z));
		});

		camera_tbl.set_function("SetFov", [&app = app](float fov) {
			app.getCamera().SetFov(fov);
		});

		camera_tbl.set_function("MoveRight", [&app = app](float speed) {
			app.getCamera().MoveRight(speed);
		});

		camera_tbl.set_function("MoveUp", [&app = app](float speed) {
			app.getCamera().MoveUp(speed);
		});

		camera_tbl.set_function("MoveForward", [&app = app](float speed) {
			app.getCamera().MoveForward(speed);
		});

		renderer_tbl.set_function("setLight", [&app = app](bool enabled) {
			app.getRenderer().setLight(enabled);
		});

		renderer_tbl.set_function("setClipping", [&app = app](bool enabled) {
			app.getRenderer().setClipping(enabled);
		});

		renderer_tbl.set_function("setMeshSize", [&app = app](float val) {
			app.getRenderer().setMeshSize(val);
		});

		renderer_tbl.set_function("setMeshShrink", [&app = app](float val) {
			app.getRenderer().setMeshShrink(val);
		});

		// app_type["test"] = sol::property(
		// 	[&app = app]() { return app.getTest(); },
		// 	[&app = app](float v) { app.setTest(v); }
		// );

		// app_type["renderer"] = sol::readonly_property(
		// 	[&app = app]() { return app.getRenderer2(); }
		// );


		// renderer_tbl.set_function("print_test", [&app = app]() {
		// });

		app_type.set_function("print_test", [&app = app]() {
			app.printTest();
		});

		app_type.set_function("reset_zoom", [&app = app]() {
			app.reset_zoom();
		});

		app_type.set_function("look_at_center", [&app = app]() {
			app.look_at_center();
		});

		app_type.set_function("setLight", [&app = app](bool b) {
			app.setLight(b);
		});

		app_type.set_function("setClipping", [&app = app](bool b) {
			app.setClipping(b);
		});

		app_type.set_function("setCullMode", [&app = app](int mode) {
			app.setCullMode(mode);
		});






	}

	inline void init() {
		if (has_init)
			init_func();
	}

	inline void key_event(int key, int scancode, int action, int mods)	{
		if (has_key_event)
			key_event_func(key, scancode, action, mods);
	}

	inline bool draw_gui() {
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
	sol::protected_function key_event_func;
	sol::protected_function draw_gui_func;

	bool has_init, has_key_event, has_draw_gui;


};