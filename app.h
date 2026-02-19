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

#include "core/shader.h"

#include "core/models/model.h"



#include "render_surface.h"

#include "core/cameras/camera.h"


#include "core/lua_script.h"

#include "core/renderers/line_renderer.h"

using namespace UM;

#include <filesystem>
namespace fs = std::filesystem;

struct App final : public IApp {

	App (const App&) = delete;
	App& operator= (const App&) = delete;

	App(Args args) : 
		args(args),
		screenWidth(1024), 
		screenHeight(768)
	{}

	// settings
	// TODO rename to windowWidth, windowHeight
	unsigned int screenWidth;
	unsigned int screenHeight;



	bool setup();
	void start();
	void clean();

	void setupColormaps();
	void setupCameras();

	// Utils functions
	Image screenshot(const std::string& filename, int targetWidth = -1, int targetHeight = -1) override;
	void quit() override;
	float getDepth(double x, double y);

	void unproject(int x, int y, float depth, glm::vec3 &p);
	glm::vec3 pickPoint(double x, double y);

	// TODO to protected
	std::string loadModel(const std::string& filename, std::string name = "") override;

	std::shared_ptr<Model> addModel(std::string type, std::string name) override {
		assert(!name.empty() && "Cannot add model with an empty name.");
		auto model = modelInstanciator.make(type);

		// Check whether renderer already exists
		if (models.contains(name))
			return models[name];

		if (!model)
			return nullptr;
		
		// model->init();
		models[name] = std::move(model);
		modelNameByIndex[model->getIndex()] = name;
		return models[name];

		// // 
		// // model->loadCallback = ([this](Model&, const std::string) -> bool {
		// // 	this->computeFarPlane();
		// // 	this->script->modelLoaded(blabla)
		// // });
	}

	void removeModel(std::string name) override {
		if (models.count(name) > 0)
			models[name]->clean();

		models.erase(name);
		// Sync
		for (auto &[i, curName] : modelNameByIndex) {
			if (curName == name) {
				modelNameByIndex.erase(i);
				break;
			}
		}
	}
	
	Model& getModel(std::string name) override {
		if (models.count(name) <= 0)
			throw std::runtime_error("Model " + name + " was not found.");
		
		return *models[name];
	}

	void focus(std::string modelName);

	void addColormap(const std::string name, const std::string filename) override;
	void removeColormap(const std::string name) override;
	void clearColormaps() override {
		colormaps.clear();
		setupColormaps();
	}

	Colormap getColormap(const std::string name) override;
	Colormap getColormap(int idx) override;



	void computeFarPlane();

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

	long pickEdge(double x, double y);
	long pick_mesh(double x, double y);
	std::vector<long> pick_vertices(double x, double y, int radius);
	std::vector<long> pick_facets(double x, double y, int radius);
	std::vector<long> pick_cells(double x, double y, int radius);

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

	std::tuple<glm::vec3, glm::vec3> computeSceneBBox();
	float computeSceneDiameter();

	// Accessors

	std::map<std::string, std::shared_ptr<Model>>& getModels() override {
		return models;
	}

	std::vector<std::shared_ptr<Model>> getChildrenOf(std::shared_ptr<Model> model) {
		std::vector<std::shared_ptr<Model>> children;
		for (auto &[k, m] : models) {
			if (m->getParent() == model) {
				children.push_back(m);
			}
		}

		return children;
	}

	inline int countModels() override {
		return models.size();
	}

	inline bool hasModel(std::string name) override {
		return models.count(name) > 0;
	}

	inline bool hasModels() override {
		return models.size() > 0;
	}

	void clearModels() override {
		for (auto &[k, m] : models) {
			m->clean();
		}

		models.clear();
		modelNameByIndex.clear();
		setSelectedModel("");
		Model::clearIndex();
	}

	bool setSelectedModel(std::string name) override {
		if (name.empty())
			return false;

		if (!hasModel(name)) {
			std::cerr << "Invalid model selection: " << name << std::endl;
			return false;
		}

		auto oldSelection = selectedModel;
		selectedModel = name;
		notifySelectedModelChanged(oldSelection, name);
		return true;
	}

	inline std::string getSelectedModel() override {
		return selectedModel;
	}

	inline Model& getCurrentModel() override {
		return *models[selectedModel];
	}

	std::shared_ptr<Model> getHoveredModel() {
		// Searching
		auto hoveredIndex = st.mesh.getHovered();

		if (modelNameByIndex.count(hoveredIndex) > 0) {
			auto name = modelNameByIndex[hoveredIndex];
			if (models.count(name) > 0) {
				return models[name];
			}
		}

		return nullptr;
	}

	std::shared_ptr<Camera> addCamera(std::string type, std::string name) override {
		assert(!name.empty() && "Cannot add camera with an empty name.");

		auto camera = cameraInstanciator.make(type);

		// Check whether renderer already exists
		if (cameras.contains(name))
			return cameras[name];

		if (!camera)
			return nullptr;

		// camera->init();
		cameras[name] = std::move(camera);
		return cameras[name];
	}

	void removeCamera(std::string name) override {
		// if (cameras.count(name) > 0)
			// cameras[name]->clean();

		cameras.erase(name);
	}

	void clearCameras() override {
		cameras.clear();
		setupCameras();
	}

	std::map<std::string, std::shared_ptr<Camera>>& getCameras() override {
		return cameras;
	}

	Camera& getCamera(std::string name) override {
		if (cameras.count(name) <= 0)
			throw std::runtime_error("Camera " + name + " was not found.");
		
		return *cameras[name];
	}

	int countCameras() override {
		return cameras.size();
	}

	bool hasCamera(std::string name) override {
		return cameras.count(name) > 0;
	}

	bool hasCameras() override {
		return cameras.size() > 0;
	}

	bool setSelectedCamera(std::string selected) override {
		if (selected.empty())
			return false;

		if (!hasCamera(selected)) {
			std::cerr << "Invalid camera selection: " << selected << std::endl;
			return false;
		}

		// Set camera to render surface
		getRenderSurface().setCamera(cameras[selected]);
		selectedCamera = selected;
		return true;
	}
	
	std::string getSelectedCamera() override {
		return selectedCamera;
	}

	Camera& getCurrentCamera() override { return *cameras[selectedCamera]; }

	std::shared_ptr<Renderer> addRenderer(std::string type, std::string name) override {
		assert(!name.empty() && "Cannot add renderer with an empty name.");

		// Check whether renderer already exists
		if (renderers.contains(name))
			return renderers[name];

		auto renderer = rendererInstanciator.make(type);

		if (!renderer)
			return nullptr;

		renderer->init();
		renderers[name] = std::move(renderer);
		return renderers[name];
	}

	void removeRenderer(std::string name) override {
		if (renderers.count(name) > 0)
			renderers[name]->clean();

		renderers.erase(name);
	}

	int countRenderers() override {
		return renderers.size();
	}

	bool hasRenderer(std::string name) override {
		return renderers.count(name) > 0;
	}

	bool hasRenderers() override {
		return renderers.size() > 0;
	}
	
	void clearRenderers() override {
		for (auto &[k, r] : renderers) {
			r->clean();
		}

		renderers.clear();
	}

	Renderer& getRenderer(std::string name) override {
		if (renderers.count(name) <= 0)
			throw std::runtime_error("Renderer " + name + " was not found.");
		
		return *renderers[name];
	}

	std::map<std::string, std::shared_ptr<Renderer>> getRenderers() override {
		return renderers;
	}



	RenderSurface &getRenderSurface() { return *renderSurfaces[0]; }
	int getSurfaceWidth() const override { return renderSurfaces[0]->width; }
	int getSurfaceHeight() const override { return renderSurfaces[0]->height; }

	InputState& getInputState() override { return st; }

	std::vector<Colormap> getColormaps() override {
		return colormaps;
	}

	int getWidth() const { return screenWidth; }
	int getHeight() const { return screenHeight; }
	double getAspectRatio() const { return static_cast<double>(screenWidth) / screenHeight; }

	void init();
	void update(float dt);
	void drawGui();

	void mouse_move(double x, double y);
	void mouse_scroll(double xoffset, double yoffset);
	void mouse_button(int button, int action, int mods);
	void key_event(int key, int scancode, int action, int mods);

	void notifyNavigationPathChanged(std::vector<std::string> &oldNavPath, std::vector<std::string>& newNavPath) {
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

	std::vector<std::string> getNavigationPath() override {
		return navPath;
	}

	void setNavigationPath(std::vector<std::string> path) override {
		notifyNavigationPathChanged(navPath, path);
		navPath = path;
	}

	void addNavigationPath(std::string pathComponent) {
		std::vector<std::string> newPath(navPath);
		newPath.push_back(pathComponent);
		notifyNavigationPathChanged(navPath, newPath);
		navPath = newPath;
	}

	void topNavigationPath() override {
		if (navPath.size() <= 0)
			return;

		auto newPath = navPath;
		newPath.erase(newPath.end() - 1);
		notifyNavigationPathChanged(navPath, newPath);
		navPath = newPath;
	}

	std::string getNavigationPathString() override {
		if (navPath.empty()) return {};

		std::ostringstream oss;
		oss << navPath[0];
		for (size_t i = 1; i < navPath.size(); ++i) 
			oss << '/' << navPath[i];

		return oss.str();
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

	// Another screenfbo
	unsigned int screenFbo;

	// OpenGL
	unsigned int fbo;
	unsigned int rbo;
	unsigned int depthAttachmentTexture;
	unsigned int texColor;
	unsigned int texCellID;
	unsigned int texFacetID;
	unsigned int texVertexID;

	unsigned int uboMatrices, uboViewport;

	unsigned int quadVAO, quadVBO;



	// display color map in good format for 2D in the UI
	std::vector<Colormap> colormaps;

	std::map<std::string, std::shared_ptr<Camera>> cameras;
	std::map<std::string, std::shared_ptr<Model>> models;
	std::map<std::string, std::shared_ptr<Renderer>> renderers;
	std::map<int, std::string> modelNameByIndex;

	std::vector<std::unique_ptr<RenderSurface>> renderSurfaces;

	std::string selectedCamera = "default";
	std::string selectedModel = "";

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

	const Instanciator<Model>& getModelInstanciator() const override { return modelInstanciator; }
	const Instanciator<Camera>& getCameraInstanciator() const override { return cameraInstanciator; }
	const Instanciator<Renderer>& getRendererInstanciator() const override { return rendererInstanciator; }


	// Current navigation path of the app
	std::vector<std::string> navPath;

	Instanciator<Model> modelInstanciator;
	Instanciator<Camera> cameraInstanciator;
	Instanciator<Renderer> rendererInstanciator;

};