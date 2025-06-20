#include "my_app.h"
#include "settings_manager.h"
#include "component_loader.h"

#include <filesystem>
namespace fs = std::filesystem;
// TODO call load model here in init function

void MyApp::init() {
	std::cout << "INIT" << std::endl;


	


	CellFacetAttribute<float> cell_highlights(hex, 0.f);

	// Engines loading...
	lua_engine.init();

	// Components loading...
	lua_console = std::make_unique<LuaConsole>(*this, lua_engine);

	// Load modules
	Settings settings;
	settings.load(args.settings_path);
	for (auto m : settings.modules) {

		// TODO check if m path exists !

		// Check for .so files
        for (const auto& entry : fs::directory_iterator(m)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".so" || entry.path().extension() == ".dll")) {
                std::cout << "Found .so file: " << entry.path().filename().string() << std::endl;
				// Load the shared library
				ComponentLoader loader;
				auto component = loader.load(entry.path().string(), *this);
				if (component) {
					// TODO see if init
					// component->init();
					scripts.push_back(std::move(component));
				} else {
					std::cout << "Failed to load component from: " << entry.path().string() << std::endl;
				}
            }
        }

		// Check for lua script
		fs::path script_path = fs::path(m) / "script.lua";
		if (fs::exists(script_path) && fs::is_regular_file(script_path)) {
			std::cout << "load module: " << m << std::endl;
			std::string script_path = m + "/script.lua";
			auto script = std::make_unique<LuaScript>(*this, script_path);
			script->init();
			scripts.push_back(std::move(script));
		}
	}

}

void MyApp::update(float dt) {
	// Continuous update

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->MoveForward(0.01f);
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->MoveForward(-0.01f);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->MoveRight(-0.01f);
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->MoveRight(0.01f);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera->MoveUp(0.01f);
	} else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera->MoveUp(-0.01f);
	}

}

void MyApp::draw_gui() {

	ImGui::Begin("Attributes", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::SetWindowPos(ImVec2(0, 300));
	ImGui::SetWindowSize(ImVec2(200, ImGui::GetWindowHeight()));


	if (ImGui::BeginCombo("##combo_colormode_selection", Renderer::colorModeStrings[hex_renderer->getColorMode()])) {
		
		for (int n = 0; n < IM_ARRAYSIZE(Renderer::colorModeStrings); n++) {
			bool isSelected = (Renderer::ColorMode)n == hex_renderer->getColorMode();
			if (ImGui::Selectable(Renderer::colorModeStrings[n], isSelected)) {
				hex_renderer->setColorMode((Renderer::ColorMode)n);

				if (hex_renderer->getColorMode() == Renderer::ColorMode::ATTRIBUTE) {
					CellAttribute<double> a(attrs[selectedAttr].second, attributes, hex, -1);
					hex_renderer->changeAttribute(a, attrs[selectedAttr].first);
				}

			}
		}
		ImGui::EndCombo();
	}


	if (hex_renderer->getColorMode() == 1) {


		const char * items[] = {"Item1", "Item2"};

		ImVec2 colormapSize(320, 35);
		if (ImGui::BeginCombo("##combo_colormaps_selection", items[selectedColormap])) {

			// Display items in the popup
			for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
				const bool isSelected = (selectedColormap == i);
				
				// Create a unique ID for each item to prevent conflicts
				ImGui::PushID(i);
				
				// Calculate total width including spacing
				float totalWidth = ImGui::CalcTextSize(items[i]).x + 320.f /* image width */ + 10.0f;
				
				// Display the item with both text and image
				if (ImGui::Selectable(items[i], isSelected)) {
					selectedColormap = i;
				}
				
				// Display the image after the text
				ImGui::Image(colormaps2D[i], colormapSize);
				
				// Clean up the ID
				ImGui::PopID();
			}

			
			
			ImGui::EndCombo();
		}

		ImGui::Image(
			(ImTextureID)(intptr_t)colormaps2D[selectedColormap], 
			colormapSize
		);


		ImGui::Text("Attribute");

		if (attrs.size() > 0) {
			if (ImGui::BeginCombo("##combo_attribute_selection", attrs[selectedAttr].second.c_str())) {
				
				for (int n = 0; n < attrs.size(); n++) {
					bool isSelected = n == selectedAttr;
					if (ImGui::Selectable(attrs[n].second.c_str(), isSelected)) {
						selectedAttr = n;
						CellAttribute<double> a(attrs[selectedAttr].second, attributes, hex, -1);
						hex_renderer->changeAttribute(a, attrs[n].first);
						std::cout << "set attr" << attrs[n].first << ":" << attrs[n].second << std::endl;
						
					}
				}
				ImGui::EndCombo();
			}
		}


	}

	ImGui::End();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open model", "Ctrl+O")) {

				// open Dialog Simple
				IGFD::FileDialogConfig config;
				config.path = ".";
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".geogram,.mesh,.json,.jpg", config);
			}
			if (ImGui::MenuItem("Quit")) {
				quit();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filename = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
			std::cout << "file path:" << directoryPath << ", file path name: " << filename << std::endl;

			std::cout << "read model..." << std::endl;
			load_model(filename);

		}
		
		// close
		ImGuiFileDialog::Instance()->Close();
	}



	ImVec2 window_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize(); 
	ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f); 
	
	ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMousePos(), cursor_radius, IM_COL32(225, 225, 255, 200), 0, 2);


	for (auto &script : scripts) {
		// if (script->status == LuaScript::SCRIPT_STATUS_OK) {
			bool success = script->draw_gui(ImGui::GetCurrentContext());
			if (!success) {

				// Clear properly ImGui

				// auto &io = ImGui::GetIO();
				// bool last_error_recovery = io.ConfigErrorRecovery;
				// io.ConfigErrorRecovery = false;
				
				// // ImGui::NewFrame();
				// // ImGui::EndFrame();

				// io.ConfigErrorRecovery = last_error_recovery;
				// // Clean up commands
				// auto draw_data = ImGui::GetDrawData();
				// if (draw_data != nullptr)
				// 	draw_data->Clear();

			}
		// }
	}

	lua_console->draw_gui(ImGui::GetCurrentContext());




}

void MyApp::key_event(int key, int scancode, int action, int mods) {
	// std::cout << "key: " << key << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		quit();
	} 
	
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		screenshot("screenshot.png");
	}

	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		camera->SetLock(true);
	} else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		camera->SetLock(false);
	}

	// if (key == GLFW_KEY_W && action == GLFW_PRESS) {
	// 	camera->MoveForward(0.01f);
	// } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
	// 	camera->MoveForward(-0.01f);
	// }
	// if (key == GLFW_KEY_A && action == GLFW_PRESS) {
	// 	camera->MoveRight(-0.01f);
	// } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
	// 	camera->MoveRight(0.01f);
	// }
	// if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
	// 	camera->MoveUp(0.01f);
	// } else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
	// 	camera->MoveUp(-0.01f);
	// }

	for (auto &script : scripts) {
		script->key_event(key, scancode, action, mods);
	}

}

void MyApp::mouse_scroll(double xoffset, double yoffset) {
	// std::cout << "scroll: " << xoffset << ", " << yoffset << std::endl;

	// Maybe move to a cameracontroller class
	if (!camera->IsLocked()) {
		scrollDelta = glm::vec2(xoffset, yoffset);
		camera->NewZoom(scrollDelta.y);
	}
	else 
		cursor_radius = std::clamp(cursor_radius + static_cast<int>(yoffset), 1, 50);

	for (auto &script : scripts) {
		script->mouse_scroll(xoffset, yoffset);
	}
}

void MyApp::mouse_button(int button, int action, int mods) {
	// std::cout << "mouse button: " << button << ", action: " << action << ", mods: " << mods << std::endl;

	for (auto &script : scripts) {
		script->mouse_button(button, action, mods);
	}
}

void MyApp::mouse_move(double x, double y) {

	// std::cout << "mouse move: " << x << ", " << y << std::endl;


	st.hovered_cell = pick_cell();

	if (st.is_cell_hovered()) {
		// std::cout << "cell hovered: " << st.h_cell << std::endl;
		auto p = pick_point(mousePos.x, mousePos.y);
		// TODO Put in setHoveredEdge function in InputState
		st.last_hovered_edge = st.hovered_edge;
		st.hovered_edge = pick_edge(hex, p, st.hovered_cell);
		// assert(st.h_edge >= 0 && st.h_edge < mesh->m.ncells() * 24);
		// std::cout << "edge hovered: " << st.h_edge << std::endl;
	} else {
		st.hovered_edge = -1;
	}


	for (auto &script : scripts) {
		script->mouse_move(x, y);
	}

}

void MyApp::mouse_drag(int button, double x, double y) {


	// if (button == GLFW_MOUSE_BUTTON_LEFT) {
	// 	camera->Move(glm::vec2(SCR_WIDTH, SCR_HEIGHT), glm::vec2(x, y), lastMousePos);
	// 	lastMousePos = glm::vec2(x, y);

	// 	auto pickIDs = pick(window, x, y, cursor_radius);
	// 	for (long pickID : pickIDs) {
	// 		if (camera->IsLocked() && pickID >= 0 && pickID < mesh->m.ncells()) {
	// 			// mesh->setHighligth(pickID, 1.f);
	// 			mesh->setFilter(pickID, true);
	// 		}
	// 	}
	// }


}