#pragma once

#include <glad/glad.h>
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

#include "shader.h"

#include "hex_renderer.h"
#include "hex_model.h"

#include "core/camera.h"
#include "cameras/arcball_camera.h"
#include "cameras/descent_camera.h"

#include "script.h"
#include "commands.h"

#include <ultimaille/all.h>

using namespace UM;

#include <filesystem>
namespace fs = std::filesystem;

struct App : public IApp {



    const float FAR_PLANE = 100.f;
    const float NEAR_PLANE = 0.1f;

    App(Args args) : 
        args(args),
        screenWidth(1024), 
        screenHeight(768), 
        leftMouse(false), 
        rightMouse(false), 
        mousePos(0,0), 
        lastMousePos(0,0), 
        scrollDelta(0,0)
    {}



    // OpenGL
    unsigned int fbo;
    unsigned int rbo;
    unsigned int screenRbo;
    unsigned int depthAttachmentTexture;
    unsigned int colorAttachmentTexture;
    unsigned int screenColorAttachmentTexture;
    unsigned int screenFbo;



    // settings
    unsigned int screenWidth;
    unsigned int screenHeight;
    




    unsigned int colormaps[2];
    unsigned int colormaps2D[2];




    void processInput(GLFWwindow *window);


    void run();

    // Frame functions
    int getWidth() override;
    int getHeight() override;

    // Utils functions
    void load_model(const std::string& filename) override;
    void screenshot(const std::string& filename) override;
    void quit();
    float get_depth(double x, double y);
    unsigned char get_depth2(double x, double y);

    void unproject(int x, int y, float depth, glm::vec3 &p);
    glm::vec3 pick_point(double x, double y);

    long pick(double xPos, double yPos);

	long pick_edge(Volume &m, glm::vec3 p0, int c) override {
		// Search nearest edge
		double min_d = std::numeric_limits<double>().max();
		long found_e = -1;
		
		for (int le = 0; le < 24; ++le) {
			long e = c * 24 + le;

			// Get local points indices of edge extremities
			int lv0 = reference_cells[1].facets[le % 4 + (le / 4 * 4)];
			int lv1 = reference_cells[1].facets[(le + 1) % 4 + (le / 4 * 4)];

			// Get global index of points
			int v0 = m.vert(c, lv0);
			int v1 = m.vert(c, lv1);

			// Get points from current edge
			vec3 p1 = m.points[v0];
			vec3 p2 = m.points[v1];
			vec3 b = (p1 + p2) * .5;
			// Compute dist from picked point to bary of edge points
			double d = (vec3(p0.x, p0.y, p0.z) - b).norm(); // TODO maybe use norm2 will give the same result

			// Keep min dist
			if (d < min_d) {
				min_d = d;
				found_e = e;
			}
		}

		return found_e;
	}

    // Pick id of vertex of the current rendering
    long pick_edge() override {
        if (st.cell.is_hovered()) {
            auto p = pick_point(mousePos.x, mousePos.y);
            // TODO get current model
            return pick_edge(getCurrentModel().getHexahedra(), p, st.cell.get_hovered());
        } else {
            return -1;
        }
    }

    // Pick id of facet of the current rendering
    long pick_facet() override;
    // Pick id of cell of the current rendering
    long pick_cell() override;

    std::vector<long> pick(double xPos, double yPos, int radius);
    
    void picking_request(Element element, glm::ivec4 region, std::function<void(std::vector<long> ids)> result_fn) {
        picking_requests.push({element, region, result_fn});
    }

    std::vector<long> extract(glm::ivec4 region);

    std::queue<std::tuple<Element, glm::ivec4, std::function<void(std::vector<long> ids)>>> picking_requests;

    // States functions
    // TODO reset_state
    // TODO save_state
    // TODO load_state
    void save_state(const std::string filename);
    void load_state(const std::string filename);

    // Rendering functions
    void setClipping(bool enabled) override;
    void setCullMode(int mode) override { cull_mode = mode; }

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

    void setSelectedModel(int selected) override {
        if (selected < 0 || selected >= models.size()) {
            std::cerr << "Invalid model index: " << selected << std::endl;
            return;
        }

        selected_renderer = selected;
    }

    int getSelectedModel() override {
        return selected_renderer;
    }

    Model& getCurrentModel() override {
        return *models[selected_renderer];
    }

    std::vector<std::unique_ptr<Camera>>& getCameras() override {
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
        selected_camera = selected;
    }
    
    int getSelectedCamera() override {
        return selected_camera;
    }

    Camera& getCamera() override { return *cameras[selected_camera]; }


    InputState& getInputState() override { return st; }

    std::vector<std::string> getPickModeStrings() const { return std::vector<std::string>(pickModeStrings, pickModeStrings + 4); }
    int getPickMode() override { return pickMode; }
    void setPickMode(Element mode) override { pickMode = mode; }
    
    std::vector<unsigned int> getColorMaps2D() override {
        return {colormaps2D[0], colormaps2D[1]};
    }

    // To override lifecycle functions
    virtual void init() = 0;
    virtual void update(float dt) = 0;
    virtual void draw_gui() = 0;
    // TODO add virtual void clean() = 0;

    // To override event functions
    virtual void mouse_move(double x, double y) = 0;
    virtual void mouse_scroll(double xoffset, double yoffset) = 0;
    virtual void mouse_button(int button, int action, int mods) = 0;
    virtual void mouse_drag(int button, double x, double y) = 0;
    virtual void mouse_dbl_click() {}
    virtual void key_event(int key, int scancode, int action, int mods) = 0;
    
    const char* pickModeStrings[4] = {"Points", "Edges", "Facets", "Cells"};




    std::vector<std::unique_ptr<Camera>> cameras;
    int selected_camera = 0;

    // Current pressed mouse button, -1 if none
    int _cur_button = -1;

    protected:
    Args args;

    GLFWwindow* window;
    glm::vec3 backgroundColor{0.05, 0.1, 0.15};
    
    std::vector<std::shared_ptr<Model>> models;
    // TODO rename! selected_model
    int selected_renderer = 0;

    glm::mat4 projection;

    std::vector<std::unique_ptr<Component>> scripts;
	InputState st;

    int _dbl_click_interval = 300 /*ms*/;

    // Mouse data
    bool leftMouse = false;
    bool rightMouse = false;
    bool dblClick = false;
    std::chrono::steady_clock::time_point lastClick;

    glm::vec2 mousePos;
    glm::vec2 lastMousePos;
    glm::vec2 lastMousePos2;
    int cursor_radius = 1;
    glm::vec2 scrollDelta;

    glm::ivec4 pickRegion;

    Element pickMode = Element::CELLS;


    // TODO make private
    int cull_mode = GL_BACK;

    private:


};