#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <glew/include/GL/glew.h>
#else
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

#include "core/app_interface.h"
#include "helpers/args_manager.h"
#include "helpers/settings_manager.h"
#include "helpers/module_loader.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ImGuiFileDialog-0.6.7/ImGuiFileDialog.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"


#include "include/stb_image.h"
#include "include/stb_image_resize2.h"
#include "include/stb_image_write.h"
#include "core/graphic_api.h"

#include <iostream>
#include <cmath>

#include "core/scene.h"
#include "core/shader.h"

#include "core/models/model.h"



#include "core/render_surface.h"

#include "core/cameras/camera.h"
#include "core/cameras/camera_collection.h"
#include "core/renderers/renderer_collection.h"
#include "core/models/model_collection.h"


#include "core/lua_script.h"

#include "core/renderers/line_renderer.h"

#include "core/navigation_path.h"

#include "core/render_system.h"

using namespace UM;

#include <filesystem>
namespace fs = std::filesystem;

struct App final : public IApp {

	App (const App&) = delete;
	App& operator= (const App&) = delete;

	App(Args args) : 
		args(args),
		windowWidth(1024), 
		windowHeight(768),
		navPath2(),
		scene(*this)
	{}

	unsigned int windowWidth;
	unsigned int windowHeight;



	bool setup();
	void start();
	void clean();


	// Utils functions
	Image screenshot(const std::string& filename, int targetWidth = -1, int targetHeight = -1) override;
	void quit() override;
	float getDepth(double x, double y);

	void unproject(Camera &camera, int x, int y, float depth, glm::vec3 &p);
	glm::vec3 pickPoint(double x, double y) override;

	
	void setupColormaps() override;
	void addColormap(const std::string name, const std::string filename) override;
	void removeColormap(const std::string name) override;

	void clearColormaps() override {
		colormaps.clear();
		setupColormaps();
	}

	std::vector<Colormap> getColormaps() override {
		return colormaps;
	}

	Colormap getColormap(const std::string name) override;
	Colormap getColormap(int idx) override;




	Scene& getScene() override { return scene; }


	void clearScene() override;

	void showOpenModelDialog() override;
	void showSaveModelDialog() override;

	// States functions
	Snapshot snapshot() override;
	void loadSnapshot() override;
	std::vector<Snapshot> listSnapshots() override;
	void saveState(const std::string filename) override;
	void loadState(const std::string filename) override;

	void loadState(json &j, const std::string path);

	long pick(double xPos, double yPos);
	std::set<long> pick(double xPos, double yPos, int radius);

	long pickEdge(double x, double y) override;
	long pick_mesh(double x, double y) override;
	std::vector<long> pick_vertices(double x, double y, int radius) override;
	std::vector<long> pick_facets(double x, double y, int radius) override;
	std::vector<long> pick_cells(double x, double y, int radius) override;

	// Rendering functions
	void setCullMode(int mode) override { cull_mode = mode; }
	bool getCull() const override { return cull; }
	
	void setCull(bool enabled) override {
		if (cull)
			glEnable(GL_CULL_FACE); 
		else 
			glDisable(GL_CULL_FACE);

		cull = enabled; 
	}


	InputState& getInputState() override { return st; }



	int getWindowWidth() const { return windowWidth; }
	int getWindowHeight() const { return windowHeight; }
	double getAspectRatio() const { return static_cast<double>(windowWidth) / windowHeight; }

	void init();
	void update(float dt);
	void drawGui();

	void mouse_move(double x, double y);
	void mouse_scroll(double xoffset, double yoffset);
	void mouse_button(int button, int action, int mods);
	void key_event(int key, int scancode, int action, int mods);

	void notifyNavigationPathChanged(NavigationPath &oldNavPath, NavigationPath& newNavPath) {
		for (auto &c : scripts) {
			c->navigationPathChanged(oldNavPath, newNavPath);
		}
	}

	void notifySelectedModelChanged(std::string oldName, std::string newName) {
		for (auto &s : scripts) {
			s->selectedModelChanged(oldName, newName);
		}
	}

	void notifySceneCleared() {
		for (auto &s : scripts) {
			s->sceneCleared();
		}
	}

	void updateCamera(float dt);

	NavigationPath getNavigationPath() override {
		return navPath2;
	}

	void setNavigationPath(std::vector<std::string> path) override {
		auto oldPath = navPath2;
		navPath2.set(path);
		notifyNavigationPathChanged(oldPath, navPath2);
	}

	void setNavigationPath(std::string strPath) override {
		auto oldPath = navPath2;
		navPath2.set(strPath);
		notifyNavigationPathChanged(oldPath, navPath2);
	}

	void addNavigationPath(std::string pathComponent) {
		auto oldPath = navPath2;
		navPath2.push(pathComponent);
		notifyNavigationPathChanged(oldPath, navPath2);
	}

	void topNavigationPath() override {
		auto oldPath = navPath2;
		navPath2.pop();
		notifyNavigationPathChanged(oldPath, navPath2);
	}

	bool isUIHovered() const override { return _isUIHovered; }

	bool isDebug() const override {
		#ifdef DEBUG
		return true;
		#else 
		return false;
		#endif
	}

	private:
	Args args;

	GLFWwindow* window;

	float fps = 0.f;
	int frameCount = 0;
	float elapsedTime = 0.000001f;

	// Another screenfbo
	unsigned int screenFbo;

	unsigned int uboMatrices, uboViewport;

	unsigned int quadVAO, quadVBO;


	unsigned int texColormaps[3];
	std::vector<Colormap> colormaps;


	Scene scene;


	std::vector<std::unique_ptr<Script>> scripts;
	InputState st;

	unsigned int eyeIcon, bugAntIcon;

	int cull_mode = GL_BACK;
	bool cull = true;

	void processInput(GLFWwindow *window);

	void loadModules(Settings &settings);
	void loadModule(fs::path modulePath) override;
	std::unique_ptr<LuaScript> loadScript(fs::path scriptPath);
	void loadCppScript(fs::path scriptPath, sol::state& state);
	void loadCppScript(fs::path scriptPath);

	void setupLayout();

	bool _isUIHovered = false;


	// Current navigation path of the app
	NavigationPath navPath2;

	RenderSystem renderSystem;

};

struct ImGuiStackState {

	ImGuiStackState(ImGuiContext *ctx) : ctx(ctx) {
		windowStackSize = ctx->CurrentWindowStack.size();
		groupStackSize = ctx->GroupStack.size();
		treeNodeStackSize = ctx->TreeNodeStack.size();
		itemFlagsStackSize = ctx->ItemFlagsStack.size();
		openPopupStackSize = ctx->OpenPopupStack.size();
		beginPopupStackSize = ctx->BeginPopupStack.size();
		currentTabBarStackSize = ctx->CurrentTabBarStack.size();
		colorStackSize = ctx->ColorStack.size();
		styleVarStackSize = ctx->StyleVarStack.size();
		fontStackSize = ctx->FontStack.size();
		disabledStackSize = ctx->DisabledStackSize;
	}

	void restore() {
		// Restore all stacks to their previous state
		// Trying to pop in reverse order of typical nesting/dependency
		while (ctx->FontStack.size() > fontStackSize) {
			ImGui::PopFont();
		}

		while (ctx->StyleVarStack.size() > styleVarStackSize) {
			ImGui::PopStyleVar();
		}

		while (ctx->ColorStack.size() > colorStackSize) {
			ImGui::PopStyleColor();
		}

		while (ctx->DisabledStackSize > disabledStackSize) {
			ImGui::EndDisabled();
		}

		while (ctx->CurrentTabBarStack.size() > currentTabBarStackSize) {
			ImGui::EndTabBar();
		}

		while (ctx->BeginPopupStack.size() > beginPopupStackSize) {
			ctx->BeginPopupStack.pop_back();
		}

		while (ctx->OpenPopupStack.size() > openPopupStackSize) {
			ctx->OpenPopupStack.pop_back();
		}

		while (ctx->ItemFlagsStack.size() > itemFlagsStackSize) {
			ctx->ItemFlagsStack.pop_back();
		}

		while (ctx->TreeNodeStack.size() > treeNodeStackSize) {
			ImGui::TreePop();
		}

		while (ctx->GroupStack.size() > groupStackSize) {
			ImGui::EndGroup();
		}

		while (ctx->CurrentWindowStack.size() > windowStackSize) {
			ImGui::End();
		}
	}

	int 
		windowStackSize,
		groupStackSize,
		treeNodeStackSize,
		itemFlagsStackSize,
		openPopupStackSize,
		beginPopupStackSize,
		currentTabBarStackSize,
		colorStackSize,
		styleVarStackSize,
		fontStackSize,
		disabledStackSize;

	private:
	ImGuiContext *ctx;

};