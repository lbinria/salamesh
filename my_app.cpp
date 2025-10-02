#include "my_app.h"
#include "helpers/settings_manager.h"
#include "helpers/module_loader.h"

#include "triangle_inspector.h"

#include <filesystem>
namespace fs = std::filesystem;

// TODO be able to load tet, surf, etc
void MyApp::loadModel(const std::string& filename) {
	#ifdef _DEBUG
	std::cout << "read model..." << std::endl;
	std::chrono::steady_clock::time_point begin_read_model = std::chrono::steady_clock::now();
	#endif

	// TODO Should deduce type here
	// TODO please do something more intelligent here !
	std::unique_ptr<Model> model;

	model = std::make_unique<HexModel>();
	bool hex_load_success = model->load(filename);
	if (!hex_load_success) {
		model = std::make_unique<TriModel>();
		bool tri_load_success = model->load(filename);
		if (!tri_load_success) {
			model = std::make_unique<QuadModel>();
			bool tri_load_success = model->load(filename);
			if (!tri_load_success) {
				model = std::make_unique<TetModel>();
				model->load(filename);
			}
		}
	}


	model->setName(std::filesystem::path(filename).stem().string() + std::to_string(models.size()));

	model->setMeshIndex(models.size());
	model->setLight(true);

	model->getMesh().setMeshShrink(0.f);
	model->getMesh().setMeshSize(0.0f);
	model->setColorMode(ColorMode::COLOR);
	
	if (model->getEdges() != nullptr)
		model->getEdges()->setVisible(false);

	models.push_back(std::move(model));	

	// Current camera look at the model
	getCamera().lookAt(getCurrentModel().getCenter());

	#ifdef _DEBUG
	// TODO display model info
	#endif

	// Notify components
	for (auto &c : components) {
		c->modelLoaded(filename);
	}

	Commands::get().add_command("app.loadModel(" + filename + ")");
}

int MyApp::addModel(std::string name) {
	auto model = std::make_unique<HexModel>();
	model->setName(name);
	model->setMeshIndex(models.size());
	models.push_back(std::move(model));
	return models.size() - 1;
}

void MyApp::removeModel(int idx) {
	models.erase(models.begin() + idx);

	// Update mesh indexes
	for (int i = idx; i < models.size(); ++i) {
		auto &m = models[i];
		m->setMeshIndex(i);
	}
}

bool MyApp::removeModel(std::string name) {
	for (int i = 0; i < models.size(); ++i) {
		auto &m = models[i];
		if (m->getName() == name) {
			// TODO fix here using removeModel(int)
			models.erase(models.begin() + i);
			return true;
		}
	}

	return false;
}

std::shared_ptr<Model> MyApp::getModelByName(std::string name) {
	for (auto &m : models) {
		if (m->getName() == name)
			return m;
	}
	return nullptr;
}

int MyApp::getIndexOfModel(std::string name) {
	for (int i = 0; i < models.size(); ++i) {
		auto &m = models[i];
		if (m->getName() == name)
			return i;
	}
	return -1;
}

void MyApp::init() {


	std::cout << "Init" << std::endl;

	// TODO Only load first for the moment
	if (!args.models.empty()) {
		for (auto m : args.models) {
			if (!fs::exists(m) || !fs::is_regular_file(m)) {
				std::cerr << "Model path given as a command line argument does not exist or is not a file: " << m << std::endl;
				continue;
			}
			loadModel(m);
		}
	}

	// ----- TESTS ------

	// // // Heavy load test (slow...)
	// // for (int i = 0; i < 10; ++i) {
	// // 	loadModel("assets/catorus_hex_attr.geogram");
	// // 	models[i+1]->setPosition(models[0]->getPosition() + glm::vec3(rand() % 10 / 10.f, rand() % 10 / 10.f, rand() % 10 / 10.f));
	// // }

	// loadModel("assets/catorus_hex_facet_attr.geogram");
	// loadModel("assets/catorus_quad.geogram");
	// loadModel("assets/catorus_tri.geogram");
	// loadModel("assets/catorus_tet.geogram");

	// // load_state("/home/tex/Desktop/state.json");

	// models[1]->setPosition(glm::vec3(1.f, 0.f, 0.f));
	// models[2]->setPosition(glm::vec3(2.f, 0.f, 0.f));
	// models[3]->setPosition(glm::vec3(3.f, 0.f, 0.f));
	// models[4]->setPosition(glm::vec3(4.f, 0.f, 0.f));

	// models[1]->setParent(models[0]);
	// models[2]->setParent(models[0]);

	// for (auto &m : getChildrenOf(models[0])) {
	// 	std::cout << "child: " << m->getName() << std::endl;
	// }

	// ----- TESTS ------



	Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
	screenShader.use();
	screenShader.setInt("screenTexture", 0);


	{
		// Get cameras start pos
		glm::vec3 camera_pos(0.f);
		if (hasModels()) {
			camera_pos = getCurrentModel().getCenter();
		}

		// Create cameras
		auto arcball_camera = std::make_shared<ArcBallCamera>("Arcball", glm::vec3(0.f, 0.f, -3.f), camera_pos, glm::vec3(0.f, 1.f, 0.f), glm::vec3(45.f, screenWidth, screenHeight));
		auto descent_camera = std::make_shared<DescentCamera>("Descent", glm::vec3(0.f, 0.f, -3.f), camera_pos, glm::vec3(0.f, 1.f, 0.f), glm::vec3(45.f, screenWidth, screenHeight));
		cameras.push_back(std::move(arcball_camera));
		cameras.push_back(std::move(descent_camera));
	}
	
	getRenderSurface().setCamera(cameras[0]);
	// renderSurfaces[1]->setCamera(cameras[1]);


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

	// Load scripts from args
	for (auto s : args.scripts) {
		if (!fs::exists(s) || !fs::is_regular_file(s)) {
			std::cerr << "Script path given as a command line argument does not exist or is not a file: " << s << std::endl;
			continue;
		}
		std::cout << "load script: " << s << std::endl;
		auto script = std::make_unique<LuaScript>(*this, s);
		script->init();
		components.push_back(std::move(script));
	}

	// ----- TEST -----
	// TODO remove just for testing manually add components
	components.push_back(std::make_unique<TriangleInspector>(*this));
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


	if (st.mouse.isLeftButton()) {
		getCamera().move(glm::vec2(screenWidth, screenHeight), st.mouse.pos, st.mouse.lastPos);
	}

	for (auto &component : components) {
		component->update(dt);
	}

}


// call each frame after NewFrame() and before Render()
void TopModePanel(int &currentMode, const std::vector<ImTextureID>& icons, ImVec2 iconSize = ImVec2(28,28)) {
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
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255,255,255,30));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255,255,255,40));
		}

		// Use ImageButton for texture icons; if you have a font icon, use ImGui::Button with Text
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0,0,0,0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0,0,0,0));
		if (ImGui::ImageButton(("btn_mode_" + std::to_string(i)).c_str(), icons[i], iconSize, ImVec2(0,0), ImVec2(1,1))) {
			currentMode = i; // select mode
		}
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();

		if (active) ImGui::PopStyleColor(3);
	}

	ImGui::End();
	ImGui::PopStyleColor(2);
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

	
	int currentMode = 0;
	TopModePanel(currentMode, {(ImTextureID)bugAntIcon});

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filename = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
			std::cout << "file path:" << directoryPath << ", file path name: " << filename << std::endl;

			std::cout << "read model..." << std::endl;
			loadModel(filename);

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



	// ImVec2 window_pos = ImGui::GetWindowPos();
	// ImVec2 window_size = ImGui::GetWindowSize(); 
	// ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
	
	ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMousePos(), st.mouse.getCursorRadius(), IM_COL32(225, 225, 255, 200), 0, 2);
	// ImGui::GetBackgroundDrawList()->AddRect(
	// 	ImVec2(pickRegion.x - pickRegion.z, pickRegion.y - pickRegion.w), 
	// 	ImVec2(pickRegion.x + pickRegion.z, pickRegion.y + pickRegion.w), 
	// 	IM_COL32(225, 225, 255, 200), 0, 2
	// );



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

	// Test
	// ImGui::Begin("Test");

	// ImGui::Text("Test window");

	// if (ImGui::Button("Highlight cells")) {
	// 	auto &model = getCurrentModel().as<HexModel>();
	// 	model.setHighlightAttr("_highlight", ElementKind::CELLS);
	// 	CellAttribute<float> hl;
	// 	hl.bind("_highlight", model.getVolumeAttributes(), model.getHexahedra());

	// 	for (auto c : model.getHexahedra().iter_cells()) {
	// 		if (c % 2 == 0)
	// 			hl[c] = 1.f;
	// 		else 
	// 			hl[c] = 0.5f;
	// 	}

	// 	model.updateHighlights();
	// }

	// if (ImGui::Button("Highlight facets")) {
	// 	auto &model = getCurrentModel().as<HexModel>();
	// 	model.setHighlightAttr("_highlight", ElementKind::CELL_FACETS);
	// 	CellFacetAttribute<float> hl;
	// 	hl.bind("_highlight", model.getVolumeAttributes(), model.getHexahedra());

	// 	for (auto c : model.getHexahedra().iter_facets()) {
	// 		if (c % 2 == 0)
	// 			hl[c] = 1.f;
	// 		else 
	// 			hl[c] = 0.5f;
	// 	}

	// 	model.updateHighlights();
	// }

	// if (ImGui::Button("Highlight points")) {

	// 	auto &model = getCurrentModel().as<HexModel>();
	// 	auto &hex = model.getHexahedra();
		
	// 	PointAttribute<float> hl;
	// 	hl.bind("_highlight", model.getVolumeAttributes(), hex);

	// 	for (auto c : hex.iter_vertices()) {
	// 		hl[c] = static_cast<float>(rand()) / RAND_MAX;
	// 	}

	// 	model.setHighlight(ElementKind::POINTS);
	// }

	// if (ImGui::Button("Unset highlights")) {
	// 	auto &model = getCurrentModel().as<HexModel>();
	// 	model.unsetHighlights();
	// }

	// if (ImGui::Button("Filter cell")) {

	// 	auto &model = getCurrentModel().as<HexModel>();
	// 	auto &hex = model.getHexahedra();
		
	// 	CellAttribute<float> fl;
	// 	fl.bind("_filter", model.getVolumeAttributes(), hex);

	// 	for (auto c : hex.iter_cells()) {
	// 		fl[c] = static_cast<float>(rand()) / RAND_MAX > .5f ? 1 : 0;
	// 		// TODO do below, to update only one element
	// 		// model.updateHighlightAt(c);
	// 	}

	// 	model.setFilter(ElementKind::CELLS);
	// }

	// if (ImGui::Button("Filter points")) {

	// 	auto &model = getCurrentModel().as<HexModel>();
	// 	auto &hex = model.getHexahedra();
		
	// 	PointAttribute<float> fl;
	// 	fl.bind("_filter", model.getVolumeAttributes(), hex);

	// 	for (auto c : hex.iter_vertices()) {
	// 		fl[c] = static_cast<float>(rand()) / RAND_MAX > .5f ? 1 : 0;
	// 	}

	// 	model.setFilter(ElementKind::POINTS);
	// }

	// if (ImGui::Button("Unset filters")) {
	// 	auto &model = getCurrentModel().as<HexModel>();
	// 	model.unsetFilters();
	// }



	// ImGui::End();

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

	for (auto &script : components) {
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


void MyApp::save_state(const std::string filename) {

	json j;
	j["cull_mode"] = cull_mode;
	// j["pick_mode"] = pickMode;
	j["selected_model"] = selectedModel;
	j["models"] = json::array();

	// Save model states
	for (auto &m : models) {
		std::string str_state = m->save_state();
		j["models"].push_back(json::parse(str_state));
	}

	// TODO Save camera(s) states


	std::ofstream ofs(filename);
	if (!ofs.is_open()) {
		std::cerr << "Failed to open file for saving state: " << filename << std::endl;
		return;
	}
	ofs << j.dump(4); // Pretty print with 4 spaces indentation
	ofs.close();
	std::cout << "State saved to: " << filename << std::endl;
}

void MyApp::load_state(const std::string filename) {
	std::ifstream ifs(filename);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open file for loading state: " << filename << std::endl;
		return;
	}
	json j;
	ifs >> j;
	ifs.close();
	std::cout << "State loaded from: " << filename << std::endl;
	cull_mode = j["cull_mode"].get<int>();
	// pickMode = (ElementKind)j["pick_mode"].get<int>();
	selectedModel = j["selected_model"].get<int>();
	// Clear current models
	models.clear();
	// Load models from state
	for (const auto& model_state : j["models"]) {
		auto model = std::make_unique<HexModel>();
		model->load_state(model_state);
		model->init();
		model->push();
		models.push_back(std::move(model));
	}
	// TODO Load camera(s) states
	std::cout << "State loaded successfully." << std::endl;
}