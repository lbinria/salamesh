#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <optional>
#include <imgui.h>

#include "app_interface.h"
#include "script.h"
#include "input_states.h"

#include "../bindings/lua_binding.h"
#include "../bindings/fs_bindings.h"
#include "../bindings/um_bindings.h"
#include "../bindings/imgui_bindings.h"
#include "../bindings/app_bindings.h"
#include "../bindings/glm_bindings.h"
#include "../bindings/camera_bindings.h"
#include "../bindings/model_bindings.h"
#include "../bindings/renderer_bindings.h"

struct LuaScript final : public Script {

	LuaScript(IApp &app, const std::string& script) : Script(app) {

		loadBindings(app);

		// TODO load / bind symbols from C++ shared lib associated to the script

		try {
			// lua.script_file(script);
			lua.safe_script_file(script);
		} catch (const sol::error& e) {
			std::cerr << "Error loading script: " << e.what() << std::endl;
		}

		// Get the functions

		// Lifecycle
		init_func = lua.get<sol::protected_function>("init");
		draw_gui_func = lua.get<sol::protected_function>("draw_gui");
		layout_gui_func = lua.get<sol::protected_function>("layout_gui");
		update_func = lua.get<sol::protected_function>("update");
		cleanup_func = lua.get<sol::protected_function>("cleanup");

		// Input events
		mouse_move_func = lua.get<sol::protected_function>("mouse_move");
		mouse_button_func = lua.get<sol::protected_function>("mouse_button");
		mouse_scroll_func = lua.get<sol::protected_function>("mouse_scroll");
		key_event_func = lua.get<sol::protected_function>("key_event");

		navigationPathChanged_func = lua.get<sol::protected_function>("navigation_path_changed");
		modelLoaded_func = lua.get<sol::protected_function>("model_loaded");
		selectedModelChanged_func = lua.get<sol::protected_function>("selected_model_changed");

		// Check whether functions exists
		has_init = init_func.valid();
		has_draw_gui = draw_gui_func.valid();
		has_layout_gui = layout_gui_func.valid();
		has_update = update_func.valid();
		has_cleanup = cleanup_func.valid();

		has_mouse_button = mouse_button_func.valid();
		has_mouse_move = mouse_move_func.valid();
		has_mouse_scroll = mouse_scroll_func.valid();
		has_key_event = key_event_func.valid();
		has_navigationPathChanged = navigationPathChanged_func.valid();
		has_modelLoaded = modelLoaded_func.valid();
		has_selectedModelChanged = selectedModelChanged_func.valid();
	}


	// TODO complete bindings
	// For a total control, I suggest to do imgui bindings (and others) "at hand":
	// - I look at SWIG quickly and it seems to be a bit heavy for what we need
	// - Don't want to create another tool to do that, that will parse code, etc. too heavy, too complicated
	// After doing it simply, we'll could think about doing better automated mecanisms
	void loadBindings(IApp &app) {

		lua.open_libraries(sol::lib::base);
		lua.open_libraries(sol::lib::string);
		lua.open_libraries(sol::lib::table);
		lua.open_libraries(sol::lib::math);
		lua.open_libraries(sol::lib::os);
		lua.open_libraries(sol::lib::io);
		lua.open_libraries(sol::lib::utf8);


		std::vector<std::unique_ptr<bindings::LuaBinding>> bindings;
		bindings.push_back(std::make_unique<bindings::FsBindings>());
		bindings.push_back(std::make_unique<bindings::ImGuiBindings>());
		bindings.push_back(std::make_unique<bindings::AppBindings>());
		bindings.push_back(std::make_unique<bindings::GlmBindings>());
		bindings.push_back(std::make_unique<bindings::CameraBindings>());
		bindings.push_back(std::make_unique<bindings::ModelBindings>());
		bindings.push_back(std::make_unique<bindings::UMBindings>());
		bindings.push_back(std::make_unique<bindings::RendererBindings>());

		for (auto &b : bindings) {
			b->loadBindings(lua, app);
		}
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

	std::vector<std::pair<std::string, std::string>> layoutGui() override {
		if (!has_layout_gui)
			return {};

		// TODO check result type and validity
		sol::object result = layout_gui_func();
		if (!result.is<sol::table>())
			return {};

		sol::table layout_table = result.as<sol::table>();
        std::vector<std::pair<std::string, std::string>> layout_result;

        // Iterate through the Lua table and convert to C++ vector of pairs
        for (const auto& [key, value] : layout_table) {
            // Ensure both key and value can be converted to strings
            std::string str_key = key.as<std::string>();
            std::string str_value = value.as<std::string>();
            layout_result.emplace_back(str_key, str_value);
        }

        return layout_result;
	}

	void update(float dt) override {
		if (has_update)
			update_func(dt);
	}

	void cleanup() override {
		if (has_cleanup)
			cleanup_func();
	}

	// Input events

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

	// App events

	void navigationPathChanged(const std::vector<std::string> &oldNavPath, const std::vector<std::string> &newNavPath) override {
		if (has_navigationPathChanged)
			navigationPathChanged_func(oldNavPath, newNavPath);
	}

	void modelLoaded(const std::string &path) override {
		if (has_modelLoaded)
			modelLoaded_func(path);
	}

	void selectedModelChanged(int idx) override {
		if (has_selectedModelChanged)
			selectedModelChanged(idx);
	}

	sol::state& getState() { return lua; }

	enum Status {
		SCRIPT_STATUS_OK,
		SCRIPT_STATUS_FAILED
	};

	Status status = SCRIPT_STATUS_OK;

	private:

	sol::state lua;
	sol::protected_function init_func;
	sol::protected_function draw_gui_func;
	sol::protected_function layout_gui_func;
	sol::protected_function update_func;
	sol::protected_function cleanup_func;

	sol::protected_function mouse_move_func;
	sol::protected_function mouse_button_func;
	sol::protected_function mouse_scroll_func;
	sol::protected_function key_event_func;
	sol::protected_function navigationPathChanged_func;
	sol::protected_function modelLoaded_func;
	sol::protected_function selectedModelChanged_func;


	bool 
		has_init, 
		has_mouse_move, 
		has_mouse_button, 
		has_mouse_scroll, 
		has_key_event, 
		has_draw_gui, 
		has_layout_gui, 
		has_update, 
		has_cleanup, 
		has_navigationPathChanged,
		has_modelLoaded,
		has_selectedModelChanged;

};