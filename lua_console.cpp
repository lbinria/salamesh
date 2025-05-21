#include "lua_console.h"
#include "lua_engine.h"
#include <imgui/imgui.h>
// #include <imgui/imgui_stdlib.h>

void LuaConsole::init() {}
void LuaConsole::setup() {}
void LuaConsole::cleanup() {}

void LuaConsole::draw_gui() {
	ImVec2 parent_size = ImGui::GetWindowSize();
	ImGui::SetNextWindowSize(ImVec2(1024, 50), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(0, 768 - 50), ImGuiCond_Always);

    // Save current style
    ImGuiStyle& style = ImGui::GetStyle();
    // Push new window padding (0 on all sides)
    style.WindowPadding = ImVec2(0, 0);

	ImGui::Begin(
		"Lua console", 
		nullptr, 
		ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_NoCollapse | 
		ImGuiWindowFlags_AlwaysAutoResize
	);

	// ImVec2 windowSize = ImVec2(ImGui::GetContentRegionMax().x, 25);

	// ImVec2 windowPos = ImVec2(0, ImGui::GetContentRegionMax().y - 25);
	// ImGui::SetWindowPos(windowPos);
	// ImGui::SetWindowSize(windowSize);

	// ImGui::InputText("##lua_input", lua_script, 512, ImGuiInputTextFlags_EnterReturnsTrue);


	if (ImGui::InputText("##lua_input", lua_script, 1024, ImGuiInputTextFlags_EnterReturnsTrue)) {
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

	style.WindowPadding = ImVec2(12, 12);
}