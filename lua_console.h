#pragma once 
#include "script.h"
#include "lua_engine.h"

struct LuaConsole : public Component {

	// Engine injection, a component can use many engines
	LuaConsole(IApp &app, LuaEngine &lua_engine) : Component(), app(app), lua_engine(lua_engine) {
		history.reserve(100);
	}

	void init() final override;
	void setup() final override;
	void cleanup() final override;
	bool draw_gui(ImGuiContext *ctx) final override;

    // Events
	void mouse_move(double x, double y) final override {}
	void mouse_button(int button, int action, int mods) final override {}
    void mouse_scroll(double xoffset, double yoffset) final override {}
    void key_event(int key, int scancode, int action, int mods) final override {}

	private:
	IApp &app;
	LuaEngine &lua_engine;
	std::vector<std::string> history;
	char lua_script[1024] = "";
	char script_history[5000] = "";
};