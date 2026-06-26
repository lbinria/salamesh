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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#include "stb_image.h"
#include "stb_image_resize2.h"
#include "stb_image_write.h"

#include <iostream>
#include <cmath>

#include "scene.h"
#include "shader.h"


#include "core/render_surface.h"

#include "camera.h"
#include "camera_collection.h"


#include "lua_script.h"

#include "shaders/line_shader.h"

#include "core/navigation_path.h"

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
		navPath(),
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

	void unproject(Camera &camera, int x, int y, float depth, vec3 &p);
	vec3 pickPoint(double x, double y) override;

	





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
	long pickMesh(double x, double y) override;
	std::vector<long> pickVertices(double x, double y, int radius) override;
	std::vector<long> pickFacets(double x, double y, int radius) override;
	std::vector<long> pickCells(double x, double y, int radius) override;

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

	void mouseMove(double x, double y);
	void mouseScroll(double xoffset, double yoffset);
	void mouseButton(int button, int action, int mods);
	void keyEvent(int key, int scancode, int action, int mods);

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
		return navPath;
	}

	void setNavigationPath(std::vector<std::string> path) override {
		auto oldPath = navPath;
		navPath.set(path);
		notifyNavigationPathChanged(oldPath, navPath);
	}

	void setNavigationPath(std::string strPath) override {
		auto oldPath = navPath;
		navPath.set(strPath);
		notifyNavigationPathChanged(oldPath, navPath);
	}

	void addNavigationPath(std::string pathComponent) {
		auto oldPath = navPath;
		navPath.push(pathComponent);
		notifyNavigationPathChanged(oldPath, navPath);
	}

	void topNavigationPath() override {
		auto oldPath = navPath;
		navPath.pop();
		notifyNavigationPathChanged(oldPath, navPath);
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
	glm::vec3 backgroundColor{0.05, 0.1, 0.15};

	float fps = 0.f;
	int frameCount = 0;
	float elapsedTime = 0.000001f;

	// Another screenfbo
	unsigned int screenFbo;

	unsigned int uboMatrices, uboViewport;

	unsigned int quadVAO, quadVBO;

	bool _isPickingActive = true;



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
	NavigationPath navPath;

};

