#include "lua_console.h"
#include "lua_engine.h"
#include <imgui/imgui.h>

void LuaConsole::init() {}
void LuaConsole::setup() {}
void LuaConsole::cleanup() {}

void LuaConsole::draw_gui() {
	ImVec2 parent_size = ImVec2(app.getWidth(), app.getHeight());
	ImGui::SetNextWindowSize(ImVec2(parent_size.x, app.getHeight() * .2f), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(0, parent_size.y - app.getHeight() * .2f), ImGuiCond_Always);

	ImGui::Begin(
		"Lua console", 
		nullptr, 
		ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_AlwaysAutoResize
	);

	ImGui::InputTextMultiline("##script_history", script_history, IM_ARRAYSIZE(script_history), ImVec2(app.getWidth() * .8f, app.getHeight() * .2f * .75f));

	if (ImGui::InputText("##script_input", lua_script, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
		// Check for key events
		auto &io = ImGui::GetIO();
	}

	ImGui::SameLine();

	if (ImGui::Button("Execute")) {
		lua_engine.execute_script(lua_script);
		history.push_back(lua_script);
		memset(lua_script, 0, sizeof(lua_script));
	}

	ImGui::End();
}