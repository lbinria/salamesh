#pragma once 
#include "tool.h"
#include "lua_engine.h"

struct LuaConsole : public Tool {

	// Engine injection, a component can use many engines
	LuaConsole(IApp &app, LuaEngine &lua_engine) : Tool(), app(app), lua_engine(lua_engine) {
		history.reserve(100);
	}

	void init() override;
	void setup() override;
	void cleanup() override;

	void draw_gui() override;

	private:
	IApp &app;
	LuaEngine &lua_engine;
	std::vector<std::string> history;
	char lua_script[1024] = "";
	char script_history[5000] = "";
};