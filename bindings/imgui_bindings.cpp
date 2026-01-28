#include "imgui_bindings.h"
#include <imgui.h>
#include "ImGuiFileDialog.h"

namespace bindings {

	void ImGuiBindings::loadBindings(sol::state &lua, IApp &app) {
		// Imgui bindings
		auto imgui = lua.create_table();

		// Window
		imgui.set_function("Begin", [](const char* name) {
			return ImGui::Begin(name);
		});

		imgui.set_function("End", []() {
			ImGui::End();
		});

		// Labels
		imgui.set_function("Text", sol::overload(
			[](const char* text) {
				ImGui::Text("%s", text);
			},
			[](const char* fmt, sol::variadic_args args) {
				ImGui::Text(fmt, args);
			}
		));
		
		imgui.set_function("TextColored", sol::overload(
			[](const float r, const float g, const float b, const float a, const char* text) {
				ImGui::TextColored(ImVec4(r, g, b, a), "%s", text);
			},
			[](const float r, const float g, const float b, const float a, const char* fmt, sol::variadic_args args) {
				ImGui::TextColored(ImVec4(r, g, b, a), fmt, args);
			}
		));

		// Buttons
		imgui.set_function("Button", [](const char* label) {
			return ImGui::Button(label);
		});

		imgui.set_function("ImageButton", [](const char* label, ImTextureID user_texture_id, ImVec2 size) {
			return ImGui::ImageButton(label, user_texture_id, size);
		});

		imgui.set_function("Checkbox", [](const char* label, sol::object v, sol::this_state s) -> std::optional<std::tuple<bool, bool>> {
			sol::state_view lua(s);

			if (v.is<bool>()) {
				bool val = v.as<bool>();
				bool sel = ImGui::Checkbox(label, &val);
				return std::make_optional(std::make_tuple(sel, val));

			} else {
				return std::nullopt;
			}

		});

		imgui.set_function("RadioButton", [](const char* label, sol::object v, sol::object vButton, sol::this_state s) -> std::optional<std::tuple<bool, int>> {
			// sol::state_view lua(s);

			if (v.is<int>() && vButton.is<int>()) {
				int val = v.as<int>();
				int valButton = vButton.as<int>();
				bool sel = ImGui::RadioButton(label, &val, valButton);
				return std::make_optional(std::make_tuple(sel, val));
			} else {
				// Here error is silent... not a good practice
				return std::nullopt;
			}

		});

		// Inputs
		imgui.set_function("InputText", [](const char* label, std::string s, int len) -> std::optional<std::tuple<bool, std::string>> {
			char *val = s.data();
			bool sel = ImGui::InputText(label, val, len);
			std::string newS = val;
			return std::make_optional(std::make_tuple(sel, newS));

		});

		imgui.set_function("InputInt", [](const char* label, sol::object v, sol::this_state s) -> std::optional<std::tuple<bool, int>> {
			sol::state_view lua(s);

			if (v.is<int>()) {
				int val = v.as<int>();
				bool sel = ImGui::InputInt(label, &val);
				return std::make_optional(std::make_tuple(sel, val));
			} else {
				return std::nullopt;
			}
		});

		imgui.set_function("InputFloat", [](const char* label, sol::object v, sol::this_state s) -> std::optional<std::tuple<bool, int>> {
			sol::state_view lua(s);

			if (v.is<int>()) {
				float val = v.as<float>();
				bool sel = ImGui::InputFloat(label, &val);
				return std::make_optional(std::make_tuple(sel, val));
			} else {
				return std::nullopt;
			}
		});

		imgui.set_function("InputFloat2", [](const char* label, sol::object v, sol::this_state s) -> std::optional<std::tuple<bool, std::array<float, 2>>> {
			sol::state_view lua(s);

			if (v.is<sol::table>()) {
				sol::table inputTable = v.as<sol::table>();
				if (inputTable.size() == 2) {
					std::array<float, 2> vals = {
						inputTable[1].get<float>(),
						inputTable[2].get<float>()
					};
				
					bool sel = ImGui::InputFloat2(label, vals.data());
					return std::make_optional(std::make_tuple(sel, vals));
				}
			}

			return std::nullopt;
		});

		imgui.set_function("InputFloat3", [](const char* label, sol::object v, sol::this_state s) -> std::optional<std::tuple<bool, std::array<float, 3>>> {
			sol::state_view lua(s);

			if (v.is<glm::vec3>()) {
				auto p = v.as<glm::vec3>();
				
				std::array<float, 3> vals = {p.x, p.y, p.z};
				bool sel = ImGui::InputFloat3(label, vals.data());
				return std::make_optional(std::make_tuple(sel, vals));
				
			}
			
			return std::nullopt;
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

		// Combo
		imgui.set_function("BeginCombo", [](const char* label, const char* preview_value) {
			return ImGui::BeginCombo(label, preview_value);
		});

		imgui.set_function("EndCombo", []() {
			ImGui::EndCombo();
		});

		// List 
		imgui.set_function("BeginListBox", sol::overload(
			[](const char* label) {
				return ImGui::BeginListBox(label);
			},
			[](const char* label, const ImVec2& size) {
				return ImGui::BeginListBox(label, size);
			}
		));

		imgui.set_function("EndListBox", []() {
			ImGui::EndListBox();
		});

		imgui.set_function("Selectable", [](const char* label, bool selected) {
			return ImGui::Selectable(label, selected);
		});

		imgui.set_function("Image", [](ImTextureID user_texture_id, ImVec2 size) {
			ImGui::Image(user_texture_id, size);
		});

		imgui.set_function("ColorEdit3", [](const char* label, sol::object color, sol::this_state s) -> std::optional<std::tuple<bool, glm::vec3>> {
			sol::state_view lua(s);

			if (color.is<glm::vec3>()) {
				auto col = color.as<glm::vec3>();
				bool changed = ImGui::ColorEdit3(label, &col.x);
				return std::make_optional(std::make_tuple(changed, col));
			} else {
				return std::nullopt;
			}
		});


		imgui.set_function("ImVec2", [](float x, float y) {
			return ImVec2(x, y);
		});

		imgui.set_function("PushID", [](int id) {
			ImGui::PushID(id);
		});

		imgui.set_function("PopID", []() {
			ImGui::PopID();
		});

		// Trees
		imgui.set_function("TreeNode", [](const char* label) {
			return ImGui::TreeNode(label);
		});

		imgui.set_function("TreeNodeEx", [](const char* label) {
			ImGuiTreeNodeFlags node_flags = 
				ImGuiTreeNodeFlags_Selected | 
				ImGuiTreeNodeFlags_OpenOnArrow | 
				ImGuiTreeNodeFlags_OpenOnDoubleClick;
			return ImGui::TreeNodeEx(label, node_flags);
		});

		imgui.set_function("TreePop", []() {
			ImGui::TreePop();
		});

		// Tabs
		imgui.set_function("BeginTabBar", sol::overload(
			[](const char* str_id) {
				return ImGui::BeginTabBar(str_id);
			},
			[](const char* str_id, const ImGuiTabBarFlags flags) {
				return ImGui::BeginTabBar(str_id, flags);
			}
		));


		imgui.set_function("EndTabBar", []() {
			ImGui::EndTabBar();
		});
		
		imgui.set_function("BeginTabItem", sol::overload(
			[](const char* label) {
				return ImGui::BeginTabItem(label);
			},
			[](const char* label, bool* p_open, const ImGuiTabItemFlags flags) {
				return ImGui::BeginTabItem(label, p_open, flags);
			}
		));

		imgui.set_function("EndTabItem", []() {
			ImGui::EndTabItem();
		});

		imgui.set_function("CollapsingHeader", sol::overload(
			[](const char* label) {
				return ImGui::CollapsingHeader(label);
			},
			[](const char* label, ImGuiTreeNodeFlags flags) {
				return ImGui::CollapsingHeader(label, flags);
			}
		));

		// Menu
		imgui.set_function("BeginMainMenuBar", []() {
			return ImGui::BeginMainMenuBar();
		});

		imgui.set_function("EndMainMenuBar", []() {
			return ImGui::EndMainMenuBar();
		});

		imgui.set_function("BeginMenu", [](const char* label) {
			return ImGui::BeginMenu(label);
		});

		imgui.set_function("EndMenu", []() {
			return ImGui::EndMenu();
		});

		imgui.set_function("MenuItem", [](const char* label) {
			return ImGui::MenuItem(label);
		});

		// Layouts
		imgui.set_function("SameLine", []() {
			ImGui::SameLine();
		});

		imgui.set_function("Separator", []() {
			ImGui::Separator();
		});

		imgui.set_function("BeginDisabled", []() {
			ImGui::BeginDisabled();
		});

		imgui.set_function("EndDisabled", []() {
			ImGui::EndDisabled();
		});

		imgui.set_function("SmallButton", [](const char* label) {
			return ImGui::SmallButton(label);
		});

		imgui.set_function("OpenDialog", [](const char* vKey, const char* vTitle, const char* vFilters) {
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = -1;
			ImGuiFileDialog::Instance()->OpenDialog(vKey, vTitle, vFilters, config);
		});

		lua["imgui"] = imgui;
	}
	
}