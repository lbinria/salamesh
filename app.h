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
#include "helpers/graphic_api.h"

#include <iostream>
#include <cmath>

#include "core/shader.h"

#include "core/models/surface_model.h"
#include "core/models/tet_model.h"
#include "core/models/hex_model.h"


#include "render_surface.h"

#include "core/cameras/camera.h"


#include "core/lua_script.h"
#include "commands.h"

using namespace UM;

#include <filesystem>
namespace fs = std::filesystem;

struct App : public IApp {


    App(Args args) : 
        args(args),
        screenWidth(1024), 
        screenHeight(768)
    {}

    // Another screenfbo
    unsigned int screenFbo;

    // OpenGL
    unsigned int fbo;
    unsigned int rbo;
    // unsigned int depthPickingRbo;
    unsigned int depthAttachmentTexture;
    unsigned int texColor;
    unsigned int texCellID;
    unsigned int texFacetID;
    unsigned int texVertexID;

    unsigned int uboMatrices, uboViewport;

    unsigned int quadVAO, quadVBO;

    // settings
    unsigned int screenWidth;
    unsigned int screenHeight;

    // TODO here need a Colormap struct {unsigned int tex; int w; int h; string name}
    // display color map in good format for 2D in the UI
    std::vector<Colormap> colormaps;


    // TO protected
    void processInput(GLFWwindow *window);

    void setup();
    void start();
    void clean();

    // Utils functions
    Image screenshot(const std::string& filename, int targetWidth = -1, int targetHeight = -1) override;
    void quit() override;
    float getDepth(double x, double y);

    void unproject(int x, int y, float depth, glm::vec3 &p);
    glm::vec3 pickPoint(double x, double y);

    // TODO to protected
    std::unique_ptr<Model> makeModel(ModelType type);
    bool loadModel(const std::string& filename) override;
    int addModel(std::string name, ModelType type) override;
    void removeModel(int idx) override;
    bool removeModel(std::string name) override;
	std::shared_ptr<Model> getModelByName(std::string name) override;
	int getIndexOfModel(std::string name) override;
	void focus(int modelIdx);

    void addColormap(const std::string name, const std::string filename) override;
    void removeColormap(const std::string name) override;
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

    std::vector<std::shared_ptr<Model>>& getModels() override {
        return models;
    }

    std::vector<std::shared_ptr<Model>> getChildrenOf(std::shared_ptr<Model> model) {
        std::vector<std::shared_ptr<Model>> children;
        for (auto &m : models) {
            if (m->getParent() == model) {
                children.push_back(m);
            }
        }

        return children;
    }

    int countModels() override {
        return models.size();
    }

    bool hasModels() override {
        return countModels() > 0;
    }

    void setSelectedModel(int selected) override {
        if (selected < 0 || selected >= models.size()) {
            std::cerr << "Invalid model index: " << selected << std::endl;
            return;
        }

        selectedModel = selected;
        notifySelectedModelChange(selected);
    }

    int getSelectedModel() override {
        return selectedModel;
    }

    Model& getCurrentModel() override {
        return *models[selectedModel];
    }

    std::shared_ptr<Model> getHoveredModel() {
        if (!st.mesh.anyHovered()) {
            return nullptr;
        } else {
            return models[st.mesh.getHovered()];
        }
    }

    std::vector<std::shared_ptr<Camera>>& getCameras() override {
        return cameras;
    }

    int countCameras() override {
        return cameras.size();
    }

    void setSelectedCamera(int selected) override {
        if (selected < 0 || selected >= cameras.size()) {
            std::cerr << "Invalid camera index: " << selected << std::endl;
            return;
        }
        // TODO see this, quick-fix
        getRenderSurface().setCamera(cameras[selected]);
        selectedCamera = selected;
    }
    
    int getSelectedCamera() override {
        return selectedCamera;
    }

    Camera& getCamera() override { return *cameras[selectedCamera]; }

    RenderSurface &getRenderSurface() { return *renderSurfaces[0]; }

    InputState& getInputState() override { return st; }

    std::vector<Colormap> getColormaps() override {
        return colormaps;
    }

    int getScreenWidth() const { return screenWidth; }
    int getScreenHeight() const { return screenHeight; }
    double getScreenAspectRatio() const { return static_cast<double>(screenWidth) / screenHeight; }

    // To override lifecycle functions
    virtual void init() = 0;
    void update(float dt);
    virtual void draw_gui() = 0;

    
    void mouse_move(double x, double y);
    void mouse_scroll(double xoffset, double yoffset);
    void mouse_button(int button, int action, int mods);
    void key_event(int key, int scancode, int action, int mods);

    void notifyNavigationPathChange(std::vector<std::string> &oldNavPath, std::vector<std::string>& newNavPath) {
        for (auto &c : scripts) {
            c->navigationPathChanged(oldNavPath, newNavPath);
        }
    }

    void notifySelectedModelChange(int idx) {
        for (auto &c : scripts) {
            c->selectedModelChanged(idx);
        }
    }

	void updateCamera(float dt);

	virtual std::unique_ptr<Camera> makeCamera(std::string type);


    std::vector<std::string> getNavigationPath() override {
        return navPath;
    }

    void setNavigationPath(std::vector<std::string> path) override {
        notifyNavigationPathChange(navPath, path);
        navPath = path;
    }

    void addNavigationPath(std::string pathComponent) {
        std::vector<std::string> newPath(navPath);
        newPath.push_back(pathComponent);
        notifyNavigationPathChange(navPath, newPath);
        navPath = newPath;
    }

    void topNavigationPath() override {
        auto newPath = navPath;
        newPath.erase(newPath.end() - 1);
        notifyNavigationPathChange(navPath, newPath);
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

	protected:
	Args args;

	GLFWwindow* window;
	glm::vec3 backgroundColor{0.05, 0.1, 0.15};

	std::vector<std::shared_ptr<Camera>> cameras;
	std::vector<std::shared_ptr<Model>> models;
	std::vector<std::unique_ptr<RenderSurface>> renderSurfaces;

	int selectedCamera = 0;
	int selectedModel = 0;

	std::vector<std::unique_ptr<Script>> scripts;
	InputState st;

	unsigned int eyeIcon = 0;
	unsigned int bugAntIcon = 0;

	int cull_mode = GL_BACK;
	bool cull = true;

	void loadModules(Settings &settings);
	void loadModule(fs::path modulePath) override;
	std::unique_ptr<LuaScript> loadScript(fs::path scriptPath);
	void loadCppScript(fs::path scriptPath, sol::state& state);
	void loadCppScript(fs::path scriptPath);

    // TODO move to private
	bool _isUIHovered = false;

	private:

	// Current navigation path of the app
	std::vector<std::string> navPath;



};