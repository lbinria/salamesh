#pragma once 
#include "tool.h"
#include "lua_engine.h"

struct LuaConsole : public Tool {

	// Engine injection, a component can use many engines
	LuaConsole(LuaEngine &lua_engine) : Tool(), lua_engine(lua_engine) {
		history.reserve(100);
	}

	void init() override;
	void setup() override;
	void cleanup() override;

	void draw_gui() override;

	private:
	LuaEngine &lua_engine;
	std::vector<std::string> history;
	char lua_script[1024] = "";
};