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

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ImGuiFileDialog-0.6.7/ImGuiFileDialog.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"


#include "include/stb_image.h"
#include "include/stb_image_write.h"

#include <iostream>
#include <cmath>

#include "core/shader.h"

#include "core/hex_renderer.h"

#include "core/tri_model.h"
#include "core/quad_model.h"
#include "core/tet_model.h"
#include "core/hex_model.h"

#include "render_surface.h"

#include "core/camera.h"
#include "cameras/arcball_camera.h"
#include "cameras/descent_camera.h"

#include "script.h"
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

    unsigned int colormaps[3];
    unsigned int colormaps2D[3];



    void processInput(GLFWwindow *window);

    void setup();
    void start();
    void clean();


    // Utils functions
    void screenshot(const std::string& filename) override;
    void quit();
    float getDepth(double x, double y);

    void unproject(int x, int y, float depth, glm::vec3 &p);
    glm::vec3 pickPoint(double x, double y);

    long pick(double xPos, double yPos);
    std::set<long> pick(double xPos, double yPos, int radius);

    long pick_vertex(double x, double y) override;
    long pick_edge(double x, double y) override;
    long pick_facet(double x, double y) override;
    long pick_cell(double x, double y) override;
    long pick_mesh(double x, double y) override;
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

    
    std::vector<unsigned int> getColorMaps2D() override {
        return {colormaps2D[0], colormaps2D[1], colormaps2D[2]};
    }

    int getScreenWidth() const { return screenWidth; }
    int getScreenHeight() const { return screenHeight; }
    double getScreenAspectRatio() const { return static_cast<double>(screenWidth) / screenHeight; }

    // To override lifecycle functions
    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual void draw_gui() = 0;
    // TODO add virtual void clean() = 0;

    // To override event functions
    virtual void mouse_move(double x, double y) = 0;
    virtual void mouse_scroll(double xoffset, double yoffset) = 0;
    virtual void mouse_button(int button, int action, int mods) = 0;
    virtual void mouse_dbl_click() {}
    virtual void key_event(int key, int scancode, int action, int mods) = 0;

    void notifyNavigationPathChange(std::vector<std::string> &oldNavPath, std::vector<std::string>& newNavPath) {
        for (auto &c : components) {
            c->navigationPathChanged(oldNavPath, newNavPath);
        }
    }

	virtual std::unique_ptr<Camera> makeCamera(std::string type) = 0;


    std::vector<std::string> getNavigationPath() {
        return navPath;
    }

    void setNavigationPath(std::vector<std::string> path) {
        notifyNavigationPathChange(navPath, path);
        navPath = path;
    }

    void addNavigationPath(std::string pathComponent) {
        std::vector<std::string> newPath(navPath);
        newPath.push_back(pathComponent);
        notifyNavigationPathChange(navPath, newPath);
        navPath = newPath;
    }

    void topNavigationPath() {
        navPath.erase(navPath.end() - 1);
    }

    std::string getNavigationPathString() {
        if (navPath.empty()) return {};

        std::ostringstream oss;
        oss << navPath[0];
        for (size_t i = 1; i < navPath.size(); ++i) 
            oss << '/' << navPath[i];

        return oss.str();
    }


    // TODO set as protected
    std::vector<std::shared_ptr<Camera>> cameras;
    // TODO rename
    int selectedCamera = 0;

    protected:
    Args args;

    GLFWwindow* window;
    glm::vec3 backgroundColor{0.05, 0.1, 0.15};
    
    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::unique_ptr<RenderSurface>> renderSurfaces;
    // TODO rename! selected_model
    int selectedModel = 0;

    // glm::mat4 projection;

    std::vector<std::unique_ptr<Component>> components;
	InputState st;

    // ElementKind pickMode = ElementKind::CELLS;


    // TODO make private
    int cull_mode = GL_BACK;
    bool cull = true;

    bool renderSurfaceWindowHovered = false;

    unsigned int eyeIcon = 0;
    unsigned int bugAntIcon = 0;
    
    private:

    // Current navigation path of the app
    std::vector<std::string> navPath;

};