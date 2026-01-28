#include "my_app.h"

#include "module_triangle_diagnostic/triangle_diagnostic_layout.h"
#include "module_triangle_diagnostic/view_component.h"
#include "module_triangle_diagnostic/triangle_inspector.h"
#include "module_triangle_diagnostic/inverted_triangle_viewer.h"

#include "core/cameras/arcball_camera.h"
#include "core/cameras/trackball_camera.h"
#include "core/cameras/descent_camera.h"


#include <filesystem>
namespace fs = std::filesystem;


void MyApp::init() {


	std::cout << "Init" << std::endl;

	Shader screenShader(sl::shadersPath("screen.vert"), sl::shadersPath("screen.frag"));
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
		std::string ext = p.extension().string();
		sl::toLower(ext);
		if (accepted.contains(ext)) {
			loadModel(p.string());
		}
		else if (p.extension() == ".lua") {
			std::cout << "load script: " << p.string() << std::endl;
			auto script = std::make_unique<LuaScript>(*this, p.string());
			script->init();
			scripts.push_back(std::move(script));
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
	loadModules(settings);
	
	// ----- TEST -----
	// TODO remove just for testing manually add scripts
	auto layoutComp = std::make_unique<TriangleDiagnosticLayout>(*this);
	auto viewComp = std::make_unique<ViewComponent>(*this);
	auto triDiagnosticComp = std::make_unique<TriangleInspector>(*this);

	layoutComp->init();
	viewComp->init();
	triDiagnosticComp->init();
	scripts.push_back(std::move(layoutComp));
	scripts.push_back(std::move(viewComp));
	scripts.push_back(std::move(triDiagnosticComp));
}

// void ModePanel(std::string modeStr) {
// 	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
// 						| ImGuiWindowFlags_NoMove
// 						| ImGuiWindowFlags_NoSavedSettings
// 						| ImGuiWindowFlags_NoFocusOnAppearing
// 						| ImGuiWindowFlags_NoNav;

// 	const float margin = 32.0f;
// 	ImGuiIO& io = ImGui::GetIO();
// 	ImVec2 displaySize = io.DisplaySize;
	
// 	ImVec2 pos = ImVec2(margin, margin + 48.f);
// 	ImVec2 size = ImVec2(displaySize.x - margin * 2.0f, 25.f);

// 	ImGui::SetNextWindowPos(pos);
// 	ImGui::SetNextWindowSize(size);
// 	ImGui::SetNextWindowBgAlpha(0.0f); // fallback for some backends

// 	ImGui::SetNextWindowDockID(0, ImGuiCond_Always); 

// 	ImGui::Begin("##NavigationPathPanel", nullptr, flags);

// 	ImGui::Text("Navigation path: %s", modeStr.c_str());

// 	ImGui::End();
// }

// // call each frame after NewFrame() and before Render()
// void MyApp::TopModePanel(int &currentMode, const std::vector<std::pair<std::string, ImTextureID>>& icons, ImVec2 iconSize) {
// 	// Layout and appearance settings
// 	const float panelHeight = 48.0f;
// 	const float margin = 32.0f;
// 	ImGuiIO& io = ImGui::GetIO();
// 	ImVec2 displaySize = io.DisplaySize;

// 	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
// 						| ImGuiWindowFlags_NoMove
// 						| ImGuiWindowFlags_NoSavedSettings
// 						| ImGuiWindowFlags_NoFocusOnAppearing
// 						| ImGuiWindowFlags_NoNav;

// 	ImVec2 pos = ImVec2(margin, margin);
// 	ImVec2 size = ImVec2(displaySize.x - margin*2.0f, panelHeight);

// 	ImGui::SetNextWindowPos(pos);
// 	ImGui::SetNextWindowSize(size);
// 	ImGui::SetNextWindowBgAlpha(0.0f); // fallback for some backends

// 	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0,0,0,0));    // fully transparent
// 	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0,0,0,0));      // no border
// 	ImGui::Begin("##TopModePanel", nullptr, flags);

// 	// Optional rounded background with semi-transparency:
// 	ImDrawList* dl = ImGui::GetWindowDrawList();
// 	ImVec2 winMin = ImGui::GetWindowPos();
// 	ImVec2 winMax = ImVec2(winMin.x + ImGui::GetWindowSize().x, winMin.y + ImGui::GetWindowSize().y);
// 	float rounding = 8.0f;
// 	// draw a subtle translucent rounded rect behind icons (customize color/alpha)
// 	dl->AddRectFilled(winMin, ImVec2(winMax.x, winMax.y), IM_COL32(20,20,20,120), rounding);
// 	dl->AddRect(winMin, ImVec2(winMax.x, winMax.y), IM_COL32(255,255,255,10), rounding);

// 	// Icon buttons
// 	ImGui::SetCursorPosY((panelHeight - iconSize.y) * 0.5f); // center vertically
// 	for (int i = 0; i < (int)icons.size(); ++i) {
// 		if (i > 0) ImGui::SameLine();

// 		// highlight active mode
// 		bool active = (currentMode == i);
// 		if (active) {
// 			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255,255,255,20));
// 			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255,255,255,200));
// 			ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255,255,255,230));
// 		}

// 		// Use ImageButton for texture icons; if you have a font icon, use ImGui::Button with Text
// 		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));

// 		if (ImGui::ImageButton(("btn_mode_" + std::to_string(i)).c_str(), icons[i].second, iconSize, ImVec2(0,0), ImVec2(1,1))) {
// 			currentMode = i; // select mode
// 			setNavigationPath({icons[i].first});
// 		}
// 		// ImGui::PopStyleColor(3);
// 		ImGui::PopStyleVar();

// 		if (active) ImGui::PopStyleColor(3);

// 		if (ImGui::IsItemHovered()) {
// 			ImGui::BeginTooltip();
// 			ImGui::Text(icons[i].first.c_str());
// 			ImGui::EndTooltip();
// 		}

// 	}

// 	ImGui::End();
// 	ImGui::PopStyleColor(2);
// }

void MyApp::setupDock() {


	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {

		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockSpaceOverViewport(dockspace_id, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

		static auto first_time = true;
		if (first_time)
		{
			first_time = false;
			// Clear out existing layout
			ImGui::DockBuilderRemoveNode(dockspace_id);

			ImGuiID top_bar_node = ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_NoTabBar);
			ImGui::DockBuilderSetNodeSize(top_bar_node, ImVec2(viewport->WorkSize.x, 30.0f)); // Thin top 

			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());



			// get id of main dock space area
			ImGuiID dockspace_main_id = dockspace_id;
			// Create a dock node for the right docked window
			ImGuiID navBar = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Left, 1.f, nullptr, &dockspace_main_id);
			ImGuiID toolBar = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Right, 1.f, nullptr, &dockspace_main_id);
			ImGuiID botBar = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Down, 0.2f, nullptr, &dockspace_main_id);

			ImGuiID leftbot = ImGui::DockBuilderSplitNode(navBar, ImGuiDir_Down, 0.25f, nullptr, &navBar);

			ImGui::DockBuilderDockWindow("Bot Bar", botBar);
			// ImGui::DockBuilderDockWindow("Left Side Bar", leftbot);
			// ImGui::DockBuilderDockWindow("Right Side Bar", toolBar);
			// ImGui::DockBuilderDockWindow("Right Side Bar2", toolBar);
			// // Position the top bar at the top
			// ImGui::DockBuilderDockWindow("TopBar", top_bar_node);

			for (auto &s : scripts) {
				auto layout = s->layoutGui();
				for (auto &l : layout) {
					if (l.second == "nav_bar")
						ImGui::DockBuilderDockWindow(l.first.c_str(), navBar);
					else if (l.second == "bot_bar")
						ImGui::DockBuilderDockWindow(l.first.c_str(), botBar);
					else if (l.second == "tool_bar")
						ImGui::DockBuilderDockWindow(l.first.c_str(), toolBar);
				}
			}

			// ImGui::DockBuilderDockWindow("Scene", left);

			ImGui::DockBuilderFinish(dockspace_id);
		}


		ImGui::Begin("Bot Bar");
		ImGui::Text("bot bar");
		ImGui::End();


	}
}



void MyApp::draw_gui() {


	setupDock();

	// Foot bar
	ImGui::SetNextWindowPos(
		ImVec2(0, ImGui::GetIO().DisplaySize.y - 30.f), 
		ImGuiCond_Always
	);
	ImGui::SetNextWindowSize(
		ImVec2(ImGui::GetIO().DisplaySize.x, 30.f)
	);

	ImGui::Begin("Foot_bar", nullptr, 
		ImGuiWindowFlags_NoMove |           // Prevent window movement
		ImGuiWindowFlags_AlwaysAutoResize | // Automatically adjust size
		ImGuiWindowFlags_NoDecoration         // Remove default title bar (optional)
	);

	ImGui::Text(getNavigationPathString().c_str());

	ImGui::End();

	
	// display
	if (ImGuiFileDialog::Instance()->Display("OpenModelDlg")) {
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
	if (ImGuiFileDialog::Instance()->Display("SaveModelAsDlg")) {
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

	_isUIHovered = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

}