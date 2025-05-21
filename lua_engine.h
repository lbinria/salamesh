#pragma once 

#include <sol/sol.hpp>
#include "states.h"
#include "app.h"
#include <imgui.h>

// TODO complete bindings
// For a total control, I suggest to do imgui bindings (and others) "at hand":
// - I look at SWIG quickly and it seems to be a bit heavy for what we need
// - Don't want to create another tool to do that, that will parse code, etc. too heavy, too complicated
// After doing it simply, we'll could think about doing better automated mecanisms
static void init_lua_state(sol::state &lua, App &app) {
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

	camera_tbl.set_function("look_at", [&app = app](float x, float y, float z) {
		app.camera->LookAt(glm::vec3(x, y, z));
	});



}

struct LuaEngine {

	// TODO must have access to core function of app to be able to expose them
	LuaEngine(App &app, InputState& st) : app(app), st(st) {
		
	}

	void init() {
		init_lua_state(lua, app);
	}

	void load_script(const std::string& script) {
		try {
			lua.script_file(script);
		} catch (const sol::error& e) {
			std::cerr << "Error loading script: " << e.what() << std::endl;
		}
	}

	void execute_script(const std::string& script) {
		try {
		auto result = lua.script(script);
		if (!result.valid()) {
			sol::error err = result;
			std::cerr << "Error executing script: " << err.what() << std::endl;
		}
		} catch (const std::exception &e) {
			std::cout << "Error executing script: " << e.what() << std::endl;
		}
	}

	void clean() {
		lua.collect_garbage();
	}

	private:
	const std::string name = "lua_engine";
	App &app;
	InputState& st;
	sol::state lua;	

};

// #include <sol/sol.hpp>
// #include <sstream>
// #include <string>

// class LineExecutor {
// private:
//     sol::state lua;
//     std::stringstream script_buffer;

// public:
//     LineExecutor() {
//         lua.open_libraries(sol::lib::base);
//     }

//     void feed_line(const std::string& line) {
//         script_buffer << line << '\n';
//     }

//     bool execute_line() {
//         std::string complete_script = script_buffer.str();
        
//         if (!complete_script.empty()) {
//             try {
//                 auto result = lua.safe_script(complete_script);
//                 if (result.valid()) {
//                     script_buffer.clear();
//                     return true;
//                 }
//             } catch (const sol::error& e) {
//                 // Incomplete statement, continue buffering
//                 return false;
//             }
//         }
//         return true;
//     }

//     void execute() {
//         std::string line;
//         while (std::getline(std::cin, line)) {
//             feed_line(line);
            
//             // Try to execute accumulated script
//             if (!execute_line()) {
//                 continue;  // Incomplete statement, wait for more lines
//             }
//         }
        
//         // Execute any remaining buffered script
//         execute_line();
//     }
// };