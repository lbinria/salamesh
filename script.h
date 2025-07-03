#pragma once 
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <optional>

#include "core/component.h"
#include "core/states.h"
// #include "app.h"
#include "core/app_interface.h"
#include <imgui.h>


// Declare app

struct LuaScript : public Component {

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
		draw_gui_func = lua.get<sol::protected_function>("draw_gui");

		mouse_move_func = lua.get<sol::protected_function>("mouse_move");
		mouse_button_func = lua.get<sol::protected_function>("mouse_button");
		mouse_scroll_func = lua.get<sol::protected_function>("mouse_scroll");
		key_event_func = lua.get<sol::protected_function>("key_event");

		// Check if function exists
		has_init = init_func.valid();
		has_draw_gui = draw_gui_func.valid();

		has_mouse_button = mouse_button_func.valid();
		has_mouse_move = mouse_move_func.valid();
		has_mouse_scroll = mouse_scroll_func.valid();
		has_key_event = key_event_func.valid();
	}


	// TODO complete bindings
	// For a total control, I suggest to do imgui bindings (and others) "at hand":
	// - I look at SWIG quickly and it seems to be a bit heavy for what we need
	// - Don't want to create another tool to do that, that will parse code, etc. too heavy, too complicated
	// After doing it simply, we'll could think about doing better automated mecanisms
	void load_bindings(IApp &app) {

		lua.open_libraries(sol::lib::base);
		lua.open_libraries(sol::lib::string);
		lua.open_libraries(sol::lib::math);
		lua.open_libraries(sol::lib::os);

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

		imgui.set_function("BeginCombo", [](const char* label, const char* preview_value) {
			return ImGui::BeginCombo(label, preview_value);
		});

		imgui.set_function("EndCombo", []() {
			ImGui::EndCombo();
		});

		imgui.set_function("Selectable", [](const char* label, bool selected) {
			return ImGui::Selectable(label, selected);
		});

		imgui.set_function("Image", [](ImTextureID user_texture_id, ImVec2 size) {
			ImGui::Image(user_texture_id, size);
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

		lua["imgui"] = imgui;


		// App bindings
		lua["app"] = &app;
		sol::usertype<IApp> app_type = lua.new_usertype<IApp>("IApp");
		// Camera bindings
		sol::usertype<ArcBallCamera> camera_tbl = lua.new_usertype<ArcBallCamera>("ArcBallCamera");
		app_type["camera"] = sol::readonly_property(&IApp::getCamera);
		// Renderer bindings
		sol::usertype<Renderer> renderer_t = lua.new_usertype<Renderer>("Renderer");



		app_type["renderers"] = sol::readonly_property(&IApp::getRenderers);
		app_type["current_renderer"] = sol::readonly_property(&IApp::getCurrentRenderer);
		
		app_type["selected_renderer"] = sol::property([](IApp &self) {
			return self.getSelectedRenderer() + 1;
		}, [](IApp &self, int selected) {
			self.setSelectedRenderer(selected);
		});

		// app_type["selected_renderer"] = sol::readonly_property([](IApp &self) {
		// 	return self.getSelectedRenderer() + 1;
		// });

		// auto renderer_tbl = lua.create_table();
		// for (int i = 0; i < app.countRenderers(); i++) {
		// 	auto &renderer = app.getRenderer(i);
		// 	renderer_tbl.set(i + 1, &renderer);
		// }
		// app_type["renderer"] = renderer_tbl;


		sol::usertype<glm::vec3> vec3_type = lua.new_usertype<glm::vec3>("vec3",
			// Constructor overloads
			sol::constructors<glm::vec3(), 
							glm::vec3(float, float, float),
							glm::vec3(const glm::vec3&)>{},

			sol::meta_function::addition, [](const glm::vec3 &a, const glm::vec3 &b) {
				return a + b;
			},

			sol::meta_function::subtraction, [](const glm::vec3 &a, const glm::vec3 &b) {
				return a - b;
			},

			sol::meta_function::multiplication, [](const glm::vec3 &a, const glm::vec3 &b) {
				return a * b;
			},

			sol::meta_function::division, [](const glm::vec3 &a, const glm::vec3 &b) {
				return a / b;
			},
			
			
			// // Property accessors
			// "x", sol::property(&glm::vec3::x),
			// "y", sol::property(&glm::vec3::y),
			// "z", sol::property(&glm::vec3::z)
			// Property accessors
			"x", sol::readonly_property(&glm::vec3::x),
			"y", sol::readonly_property(&glm::vec3::y),
			"z", sol::readonly_property(&glm::vec3::z)
		);

		vec3_type.set_function("to_string", [](glm::vec3 &v) {
			return std::string("vec3(") + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
		});

		vec3_type.set_function("getX", [](glm::vec3 &v) {
			return v.x;
		});
		// // WTF ?! I need to add this to be able to access the properties
		// vec3_type["x"] = sol::readonly_property(&glm::vec3::x);
		// vec3_type["y"] = sol::readonly_property(&glm::vec3::y);
		// vec3_type["z"] = sol::readonly_property(&glm::vec3::z);
		glm::vec3 v;
		v.x = 0.0f;


		camera_tbl.set_function("LookAt", [&app = app](ArcBallCamera &camera, float x, float y, float z) {
			app.getCamera().LookAt(glm::vec3(x, y, z));
		});

		camera_tbl.set_function("GetFovAndScreen", [&app = app]() {
			auto fov_and_screen = app.getCamera().GetFovAndScreen();
			return std::make_tuple(fov_and_screen.x, fov_and_screen.y, fov_and_screen.z);
		});

		camera_tbl.set_function("SetFov", &ArcBallCamera::SetFov);
		camera_tbl.set_function("MoveRight", &ArcBallCamera::MoveRight);
		camera_tbl.set_function("MoveUp", &ArcBallCamera::MoveUp);
		camera_tbl.set_function("MoveForward", &ArcBallCamera::MoveForward);


		renderer_t["light"] = sol::property(
			&Renderer::getLight,
			&Renderer::setLight
		);

		renderer_t.set_function("getLight", &Renderer::getLight);
		renderer_t.set_function("setLight", &Renderer::setLight);

		renderer_t["is_light_follow_view"] = sol::property(
			&Renderer::getLightFollowView,
			&Renderer::setLightFollowView
		);

		renderer_t["clipping"] = sol::property(
			&Renderer::getClipping,
			&Renderer::setClipping
		);

		renderer_t.set_function("getClipping", &Renderer::getClipping);
		renderer_t.set_function("setClipping", &Renderer::setClipping);

		renderer_t["clipping_plane_point"] = sol::property(
			&Renderer::getClippingPlanePoint,
			&Renderer::setClippingPlanePoint
		);

		renderer_t["clipping_plane_normal"] = sol::property(
			&Renderer::getClippingPlaneNormal,
			&Renderer::setClippingPlaneNormal
		);

		renderer_t["invert_clipping"] = sol::property(
			&Renderer::getInvertClipping,
			&Renderer::setInvertClipping
		);

		renderer_t["meshSize"] = sol::property(
			&Renderer::getMeshSize,
			&Renderer::setMeshSize
		);

		renderer_t.set_function("getMeshSize", &Renderer::getMeshSize);
		renderer_t.set_function("setMeshSize", &Renderer::setMeshSize);

		renderer_t["meshShrink"] = sol::property(
			&Renderer::getMeshShrink,
			&Renderer::setMeshShrink
		);

		renderer_t.set_function("getMeshShrink", &Renderer::getMeshShrink);
		renderer_t.set_function("setMeshShrink", &Renderer::setMeshShrink);

		renderer_t["color_mode_strings"] = sol::readonly_property(
			&Renderer::getColorModeStrings
		);

		renderer_t["color_mode"] = sol::property(
			&Renderer::getColorMode,
			&Renderer::setColorMode
		);



		// auto attrs_tbl = lua.create_table();
		// for (auto &attr : app.getRenderer().getAttrs()) {
		// 	sol::table item = lua.create_table();
		// 	item.add(std::get<0>(attr)); // First element (name)
		// 	item.add(std::get<1>(attr)); // Second element (element)
		// 	attrs_tbl.add(item);
		// }
		// renderer_t["attrs"] = attrs_tbl;



		renderer_t["attrs"] = sol::readonly_property([&lua = lua](Renderer &self) {
			sol::table attrs_tbl = lua.create_table();
			for (auto &attr : self.getAttrs()) {
				sol::table item = lua.create_table();
				item.set(1, std::get<0>(attr)); // First element (name)
				item.set(2, std::get<1>(attr)); // Second element (element)
				attrs_tbl.add(item);
			}
			return attrs_tbl;
		});


		renderer_t["selected_attr"] = sol::property(
			&Renderer::getSelectedAttr,
			&Renderer::setSelectedAttr
		);

		renderer_t["selected_colormap"] = sol::property(
			&Renderer::getSelectedColormap,
			&Renderer::setSelectedColormap
		);

		app_type["pick_mode_strings"] = sol::readonly_property(
			&IApp::getPickModeStrings
		);

		app_type["pick_mode"] = sol::property(
			&IApp::getPickMode,
			&IApp::setPickMode
		);

		app_type["colormaps_2d"] = sol::readonly_property(
			&IApp::getColorMaps2D
		);

		// std::vector<std::string> v = {"a", "b", "c"};
		// lua["pick_mode_strings"] = sol::as_table(v);
		// app_type["pp"] = sol::readonly_property([]() {sol::as_table(v); });
		// // Copy values
		// for (size_t i = 0; i < app.getPickModeStrings().size(); ++i) {
		// 	pick_mode_strings_tbl[i + 1] = app.getPickModeStrings()[i];
		// }

		// app_type.set_function("load_model", &IApp::load_model);
		// app_type.set_function("screenshot", &IApp::screenshot);
		app_type.set_function("load_model", [&app = app](const std::string& filename) {
			app.load_model(filename);
		});
		
		app_type.set_function("screenshot", [&app = app](const std::string& filename) {
			app.screenshot(filename);
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

	void init() final override {
		if (has_init)
			init_func();
	}

	void setup() final override {}
	void cleanup() final override {}

	void mouse_move(double x, double y) {
		if (has_mouse_move)
			mouse_move_func(x, y);
	}

	void mouse_button(int button, int action, int mods) {
		if (has_mouse_button)
			mouse_button_func(button, action, mods);
	}

	void mouse_scroll(double xoffset, double yoffset) {
		if (has_mouse_scroll)
			mouse_scroll_func(xoffset, yoffset);
	}

	inline void key_event(int key, int scancode, int action, int mods) final override {
		if (has_key_event)
			key_event_func(key, scancode, action, mods);
	}

	inline bool draw_gui(ImGuiContext *ctx) final override {
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
	sol::protected_function mouse_move_func;
	sol::protected_function mouse_button_func;
	sol::protected_function mouse_scroll_func;
	sol::protected_function key_event_func;
	sol::protected_function draw_gui_func;

	bool has_init, has_mouse_move, has_mouse_button, has_mouse_scroll, has_key_event, has_draw_gui;


};