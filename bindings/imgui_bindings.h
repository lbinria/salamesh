#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <imgui.h>

namespace bindings {

	struct ImGuiBindings {

		static void loadBindings(sol::state &lua) {

			// Imgui bindings
			auto imgui = lua.create_table();

			// Window
			imgui.set_function("Begin", [](const char* name) {
				return ImGui::Begin(name);
			});

			imgui.set_function("End", []() {
				ImGui::End();
			});

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

			// Inputs
			imgui.set_function("InputText", [](const char* label, char* buf, size_t buf_size) {
				return ImGui::InputText(label, buf, buf_size);
			});

			imgui.set_function("Button", [](const char* label) {
				return ImGui::Button(label);
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

				// if (color.is<ImVec4>()) {
				// 	ImVec4 col = color.as<ImVec4>();
				// 	bool changed = ImGui::ColorEdit3(label, &col.x);
				// 	return std::make_optional(std::make_tuple(changed, col));
				// } else {
				// 	return std::nullopt;
				// }

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

			imgui.set_function("TreePop", []() {
				ImGui::TreePop();
			});

			// Tabs
			// imgui.set_function("BeginTabBar", [](const char* str_id, const ImGuiTabBarFlags flags) {
			// 	return ImGui::BeginTabBar(str_id, flags);
			// });

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
			// imgui.set_function("BeginTabItem", [](const char* label, bool* p_open, const ImGuiTabItemFlags flags) {
			// 	return ImGui::BeginTabItem(label, p_open, flags);
			// });
			
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

			imgui.set_function("SameLine", []() {
				ImGui::SameLine();
			});

			imgui.set_function("Separator", []() {
				ImGui::Separator();
			});

			imgui.set_function("SmallButton", [](const char* label) {
				return ImGui::SmallButton(label);
			});

			lua["imgui"] = imgui;

		}

	};

}