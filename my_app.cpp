#include "my_app.h"
#include "helpers/settings_manager.h"
#include "helpers/module_loader.h"

#include "module_triangle_diagnostic/triangle_diagnostic_layout.h"
#include "module_triangle_diagnostic/view_component.h"
#include "module_triangle_diagnostic/triangle_inspector.h"
#include "module_triangle_diagnostic/inverted_triangle_viewer.h"

#include "cameras/arcball_camera.h"
#include "cameras/trackball_camera.h"
#include "cameras/descent_camera.h"


#include <filesystem>
namespace fs = std::filesystem;


void MyApp::init() {


	std::cout << "Init" << std::endl;

	Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
	screenShader.use();
	screenShader.setInt("screenTexture", 0);


	{
		// Create cameras
		// auto arcball_camera = std::make_shared<ArcBallCamera>("Arcball");
		auto trackball_camera = std::make_shared<TrackBallCamera>("Trackball");
		auto descent_camera = std::make_shared<DescentCamera>("Descent");
		// cameras.push_back(std::move(arcball_camera));
		cameras.push_back(std::move(trackball_camera));
		cameras.push_back(std::move(descent_camera));
	}
	
	getRenderSurface().setCamera(cameras[0]);
	// renderSurfaces[1]->setCamera(cameras[1]);


	// Load model
	// Check for files given in args (is it models, scripts ?)
	std::set<std::string> accepted{".obj", ".mesh", ".geogram"};
	for (auto &p : args.paths) {
		if (accepted.contains(p.extension().string())) {
			loadModel(p.string());
		}
		else if (p.extension() == ".lua") {
			std::cout << "load script: " << p.string() << std::endl;
			auto script = std::make_unique<LuaScript>(*this, p.string());
			script->init();
			components.push_back(std::move(script));
		}
		else if (p.extension() == ".json") {
			// Check internal format of json file (state file, settings...)
			std::ifstream ifs(p);
			if (!ifs.is_open()) {
				std::cerr << "Failed to open file given as argument: " << p.string() << std::endl;
				continue;;
			}

			json j;
			ifs >> j;
			ifs.close();

			if (j["header"]["type"] == "state") {
				loadState(j, p.string());
			}
		}
	}

	// Focus last loaded model
	if (models.size() > 0)
		focus(models.size() - 1);

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
					// TODO see if init, I THINK IT  MUST BE INIT init is app init, but check module (collapse, painting before change)
					// component->init();
					components.push_back(std::move(component));
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
			components.push_back(std::move(script));
		}
	}

	// ----- TEST -----
	// TODO remove just for testing manually add components
	auto layoutComp = std::make_unique<TriangleDiagnosticLayout>(*this);
	auto viewComp = std::make_unique<ViewComponent>(*this);
	auto triDiagnosticComp = std::make_unique<TriangleInspector>(*this);
	// auto invertedTriangleViewerComp = std::make_unique<InvertedTriangleViewer>(*this);

	layoutComp->init();
	viewComp->init();
	triDiagnosticComp->init();
	// invertedTriangleViewerComp->init();
	components.push_back(std::move(layoutComp));
	components.push_back(std::move(viewComp));
	components.push_back(std::move(triDiagnosticComp));
	// components.push_back(std::move(invertedTriangleViewerComp));
}

// Continuous update
void MyApp::update(float dt) {
	
	if (!isUIHovered)
		updateCamera(dt);

	for (auto &component : components) {
		component->update(dt);
	}

}

void MyApp::updateCamera(float dt) {

	float speed = 0.01f;
	if (countModels() > 0) {
		speed = getCurrentModel().getRadius() * 0.5f * dt;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		getCamera().moveForward(speed);
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		getCamera().moveForward(-speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		getCamera().moveRight(-speed);
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		getCamera().moveRight(speed);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		getCamera().moveUp(speed);
	} else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		getCamera().moveUp(-speed);
	}


	if (st.mouse.isLeftButton()) {
		// getCamera().move(st.mouse.delta);
		getCamera().move(st.mouse.lastPos, st.mouse.pos);
	}

	if (st.mouse.isRightButton()) {
		auto arcball = dynamic_cast<ArcBallCamera*>(&getCamera());
		if (arcball) {
			arcball->movePan(st.mouse.delta);
		}
		else {
			auto trackball = dynamic_cast<TrackBallCamera*>(&getCamera());
			if (trackball)
				trackball->movePan(st.mouse.delta);
		}
	}

}


void ModePanel(std::string modeStr) {
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
						| ImGuiWindowFlags_NoMove
						| ImGuiWindowFlags_NoSavedSettings
						| ImGuiWindowFlags_NoFocusOnAppearing
						| ImGuiWindowFlags_NoNav;

	const float margin = 32.0f;
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 displaySize = io.DisplaySize;

	ImVec2 pos = ImVec2(margin, margin + 48.f);
	ImVec2 size = ImVec2(displaySize.x - margin * 2.0f, 25.f);

	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowBgAlpha(0.0f); // fallback for some backends

	ImGui::SetNextWindowDockID(0, ImGuiCond_Always); 

	ImGui::Begin("##NavigationPathPanel", nullptr, flags);

	ImGui::Text("Navigation path: %s", modeStr.c_str());

	ImGui::End();
}

// call each frame after NewFrame() and before Render()
void MyApp::TopModePanel(int &currentMode, const std::vector<std::pair<std::string, ImTextureID>>& icons, ImVec2 iconSize) {
	// Layout and appearance settings
	const float panelHeight = 48.0f;
	const float margin = 32.0f;
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 displaySize = io.DisplaySize;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
						| ImGuiWindowFlags_NoMove
						| ImGuiWindowFlags_NoSavedSettings
						| ImGuiWindowFlags_NoFocusOnAppearing
						| ImGuiWindowFlags_NoNav;

	ImVec2 pos = ImVec2(margin, margin);
	ImVec2 size = ImVec2(displaySize.x - margin*2.0f, panelHeight);

	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	ImGui::SetNextWindowBgAlpha(0.0f); // fallback for some backends

	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0,0,0,0));    // fully transparent
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0,0,0,0));      // no border
	ImGui::Begin("##TopModePanel", nullptr, flags);

	// Optional rounded background with semi-transparency:
	ImDrawList* dl = ImGui::GetWindowDrawList();
	ImVec2 winMin = ImGui::GetWindowPos();
	ImVec2 winMax = ImVec2(winMin.x + ImGui::GetWindowSize().x, winMin.y + ImGui::GetWindowSize().y);
	float rounding = 8.0f;
	// draw a subtle translucent rounded rect behind icons (customize color/alpha)
	dl->AddRectFilled(winMin, ImVec2(winMax.x, winMax.y), IM_COL32(20,20,20,120), rounding);
	dl->AddRect(winMin, ImVec2(winMax.x, winMax.y), IM_COL32(255,255,255,10), rounding);

	// Icon buttons
	ImGui::SetCursorPosY((panelHeight - iconSize.y) * 0.5f); // center vertically
	for (int i = 0; i < (int)icons.size(); ++i) {
		if (i > 0) ImGui::SameLine();

		// highlight active mode
		bool active = (currentMode == i);
		if (active) {
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255,255,255,20));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255,255,255,200));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255,255,255,230));
		}

		// Use ImageButton for texture icons; if you have a font icon, use ImGui::Button with Text
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));

		if (ImGui::ImageButton(("btn_mode_" + std::to_string(i)).c_str(), icons[i].second, iconSize, ImVec2(0,0), ImVec2(1,1))) {
			currentMode = i; // select mode
			setNavigationPath({icons[i].first});
		}
		// ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();

		if (active) ImGui::PopStyleColor(3);

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text(icons[i].first.c_str());
			ImGui::EndTooltip();
		}

	}

	ImGui::End();
	ImGui::PopStyleColor(2);
}

void MyApp::draw_gui() {

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open model")) {

				// open Dialog Simple
				IGFD::FileDialogConfig config;
				config.path = ".";
				config.countSelectionMax = -1;
				// config.flags = ImGuiFileDialogFlags_Mul
				ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", "All supported mesh files {.geogram, .mesh, .obj},.geogram,.mesh,.obj,.json", config);
			}

			if (ImGui::MenuItem("Save model as")) {
				// open Dialog Simple
				IGFD::FileDialogConfig config;
				config.path = ".";
				config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
				ImGuiFileDialog::Instance()->OpenDialog("SaveModelAs", "Save as", ".geogram,.mesh,.obj", config);
			}

			if (ImGui::MenuItem("Clear scene")) {
				clearScene();
			}

			if (ImGui::MenuItem("Save state")) {
				saveState("state.json");
			}

			if (ImGui::MenuItem("Load state")) {
				loadState("state.json");
			}

			if (ImGui::MenuItem("Quit")) {
				quit();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	
	static int currentMode = -1;
	TopModePanel(currentMode, {{"view", (ImTextureID)eyeIcon}, {"diagnostic", (ImTextureID)bugAntIcon}});
	ModePanel(getNavigationPathString());


	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

			std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::cout << "directory path:" << directoryPath << std::endl;

			auto selection = ImGuiFileDialog::Instance()->GetSelection();
			
			for (auto &kv : selection)
			{
				std::string filename = kv.first;
				std::string fullpath = kv.second;
				std::cout << "filename: " << filename << ", fullpath: " << fullpath << std::endl;
				std::cout << "read model..." << std::endl;
				loadModel(fullpath);
			}

			// Focus last loaded model
			if (models.size() > 0)
				focus(models.size() - 1);
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
			getCurrentModel().saveAs(filename);
		}
		
		// close
		ImGuiFileDialog::Instance()->Close();
	}

	
	ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMousePos(), st.mouse.getCursorRadius(), IM_COL32(225, 225, 255, 200), 0, 2);



	for (auto &script : components) {
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



	isUIHovered = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

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
		Camera &refCam = getCamera();
		setSelectedCamera(0);
		if (countModels() > 0)
			getCamera().copy(refCam, getCurrentModel().bbox());
	} else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		Camera &refCam = getCamera();
		setSelectedCamera(1);
		if (countModels() > 0)
			getCamera().copy(refCam, getCurrentModel().bbox());
	}

	for (auto &script : components) {
		script->key_event(key, scancode, action, mods);
	}

}

void MyApp::mouse_scroll(double xoffset, double yoffset) {
	if (isUIHovered)
		return;

	// Maybe move to a cameracontroller class
	if (!getCamera().isLocked()) {
		st.mouse.scrollDelta = glm::vec2(xoffset, yoffset);
		getCamera().zoom(st.mouse.scrollDelta.y);
	}
	else 
		st.mouse.setCursorRadius(st.mouse.getCursorRadius() + static_cast<int>(yoffset));

	for (auto &script : components) {
		script->mouse_scroll(xoffset, yoffset);
	}
}

void MyApp::mouse_button(int button, int action, int mods) {

	for (auto &script : components)
		script->mouse_button(button, action, mods);
}

void MyApp::mouse_move(double x, double y) {

	long pick_mesh_id = pick_mesh(x, y);
	if (pick_mesh_id >= 0)
		st.mesh.setHovered({pick_mesh_id});
	else 
		st.mesh.setHovered({});

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

	for (auto &script : components) {
		script->mouse_move(x, y);
	}

}

std::unique_ptr<Camera> MyApp::makeCamera(std::string type) {
	// Register cameras
	if (type == "ArcBallCamera")
		return std::make_unique<ArcBallCamera>();
	else if (type == "DescentCamera")
		return std::make_unique<DescentCamera>();
	else if (type == "TrackBallCamera")
		return std::make_unique<TrackBallCamera>();
	else {
		std::cerr 
			<< "Unable to make camera of type " 
			<< type 
			<< ", maybe you should override `makeCamera` to add the construction of your custom camera class ?" 
			<< std::endl;

		return nullptr;
	}
}