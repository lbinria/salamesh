#include "my_app.h"
#include "helpers/settings_manager.h"
#include "helpers/module_loader.h"

#include <filesystem>
namespace fs = std::filesystem;
// TODO call load model here in init function

void MyApp::init() {
	std::cout << "INIT" << std::endl;

	// Engines loading...
	

	// Components loading...

	// Load modules
	Settings settings;
	settings.load(args.settings_path);
	for (auto m : settings.modules) {

		if (!fs::exists(m)) {
			std::cerr << "Module path does not exist: " << m << std::endl;
			continue;
		}

		// Check for .so files
        for (const auto& entry : fs::directory_iterator(m)) {
            if (entry.is_regular_file() && (entry.path().extension() == ".so" || entry.path().extension() == ".dll")) {
                std::cout << "Found .so file: " << entry.path().filename().string() << std::endl;
				// Load the shared library
				ModuleLoader loader;
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
		getCamera().moveForward(0.01f);
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		getCamera().moveForward(-0.01f);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		getCamera().moveRight(-0.01f);
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		getCamera().moveRight(0.01f);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		getCamera().moveUp(0.01f);
	} else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		getCamera().moveUp(-0.01f);
	}


	Hexahedra &hex = getCurrentModel().getHexahedra();

	if (st.mouse.isLeftButton()) {
		
		getCamera().move(glm::vec2(screenWidth, screenHeight), st.mouse.pos, st.mouse.lastPos);

		// auto pickIDs = pick_cells(st.mouse.pos.x, st.mouse.pos.y, st.mouse.getCursorRadius());
		// for (long pickID : pickIDs) {
		// 	if (getCamera().isLocked() && pickID >= 0 && pickID < hex.ncells()) {
		// 		models[selected_renderer]->setFilter(pickID, true);
		// 	}
		// }

	}

	// if (dblClick) {
	// 	double xPos, yPos;
	// 	glfwGetCursorPos(window, &xPos, &yPos);
	// 	long pickID = pick(xPos, yPos);

	// 	if (pickID > 0 && pickID < hex.ncells()) {
	// 		Volume::Cell c(hex, pickID);
	// 		auto p = 
	// 			(c.vertex(0).pos() + 
	// 			c.vertex(1).pos() +
	// 			c.vertex(2).pos() +
	// 			c.vertex(3).pos() +
	// 			c.vertex(4).pos() +
	// 			c.vertex(5).pos() +
	// 			c.vertex(6).pos() +
	// 			c.vertex(7).pos()) / 8.;

	// 		getCamera().lookAt(glm::vec3(p.x, p.y, p.z));

	// 		std::cout << "dblClick on cell: " << pickID << std::endl;
	// 	}
	// }

	for (auto &script : scripts) {
		script->update(dt);
	}

}

bool firstLoop = true;

void MyApp::draw_gui() {

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open model")) {

				// open Dialog Simple
				IGFD::FileDialogConfig config;
				config.path = ".";
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".geogram,.mesh,.json,.jpg", config);
			}

			if (ImGui::MenuItem("Save model as")) {
				// open Dialog Simple
				IGFD::FileDialogConfig config;
				config.path = ".";
				ImGuiFileDialog::Instance()->OpenDialog("SaveModelAs", "Save as", ".geogram,.mesh", config);
			}

			if (ImGui::MenuItem("Save state")) {
				std::string filename = "state.json";
				save_state(filename);
				std::cout << "State saved to: " << filename << std::endl;
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

	// display save dialog
	if (ImGuiFileDialog::Instance()->Display("SaveModelAs")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filename = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
			std::cout << "file path:" << directoryPath << ", file path name: " << filename << std::endl;
			std::cout << "save model..." << std::endl;
			getCurrentModel().save_as(filename);
		}
		
		// close
		ImGuiFileDialog::Instance()->Close();
	}



	// ImVec2 window_pos = ImGui::GetWindowPos();
	// ImVec2 window_size = ImGui::GetWindowSize(); 
	// ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
	
	ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMousePos(), st.mouse.getCursorRadius(), IM_COL32(225, 225, 255, 200), 0, 2);
	// ImGui::GetBackgroundDrawList()->AddRect(
	// 	ImVec2(pickRegion.x - pickRegion.z, pickRegion.y - pickRegion.w), 
	// 	ImVec2(pickRegion.x + pickRegion.z, pickRegion.y + pickRegion.w), 
	// 	IM_COL32(225, 225, 255, 200), 0, 2
	// );



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


}

void MyApp::key_event(int key, int scancode, int action, int mods) {
	
	bool ctrlPressed = (mods & GLFW_MOD_CONTROL) != 0;
	if (ctrlPressed && key == GLFW_KEY_A && action == GLFW_PRESS) {
		quit();
	}
	
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		screenshot("screenshot.png");
	}

	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		getCamera().setLock(true);
	} else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		getCamera().setLock(false);
	}

	// Switch cameras with 1 and 2 keys
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		auto pos = getCamera().getEye();
		auto fov = getCamera().getFov();
		setSelectedCamera(0); 
		getCamera().setEye(pos);
		getCamera().setFov(fov);
	} else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		auto pos = getCamera().getEye();
		auto lookAt = getCamera().getLookAt();
		auto fov = getCamera().getFov();
		setSelectedCamera(1); 
		getCamera().setEye(pos);
		getCamera().lookAt(lookAt);
		getCamera().setFov(fov);
	}

	for (auto &script : scripts) {
		script->key_event(key, scancode, action, mods);
	}

}

void MyApp::mouse_scroll(double xoffset, double yoffset) {
	// std::cout << "scroll: " << xoffset << ", " << yoffset << std::endl;

	// Maybe move to a cameracontroller class
	if (!getCamera().isLocked()) {
		st.mouse.scrollDelta = glm::vec2(xoffset, yoffset);
		getCamera().zoom(st.mouse.scrollDelta.y);
	}
	else 
		st.mouse.setCursorRadius(st.mouse.getCursorRadius() + static_cast<int>(yoffset));

	for (auto &script : scripts) {
		script->mouse_scroll(xoffset, yoffset);
	}
}

void MyApp::mouse_button(int button, int action, int mods) {

	for (auto &script : scripts)
		script->mouse_button(button, action, mods);
}

void MyApp::mouse_move(double x, double y) {

	st.vertex.setHovered(pick_vertices(x, y, st.mouse.getCursorRadius()));
	st.facet.setHovered(pick_facets(x, y, st.mouse.getCursorRadius()));
	st.cell.setHovered(pick_cells(x, y, st.mouse.getCursorRadius()));

	if (glm::dot(st.mouse.pos, st.mouse.lastPos) > 4) {
		auto edge = pick_edge(x, y);
		if (edge >= 0) 
			st.edge.setHovered({edge});
		else
			st.edge.setHovered({});
	}

	for (auto &script : scripts) {
		script->mouse_move(x, y);
	}

}