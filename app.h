#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/app_interface.h"
#include "args_manager.h"

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
#include "wireframe.h"
#include "hex_model.h"

#include "core/arcball_camera.h"

#include "script.h"

#include <ultimaille/all.h>



using namespace UM;

#include <filesystem>
namespace fs = std::filesystem;

// void mouse_callback(GLFWwindow* window, double xpos, double ypos);

struct App : public IApp {



    const float FAR_PLANE = 100.f;
    const float NEAR_PLANE = 0.1f;

    App(Args args) : 
        args(args),
        SCR_WIDTH(1024), 
        SCR_HEIGHT(768), 
        leftMouse(false), 
        rightMouse(false), 
        mousePos(0,0), 
        lastMousePos(0,0), 
        scrollDelta(0,0)
    {}


    std::unique_ptr<ArcBallCamera> camera;

    unsigned int fbo;
    unsigned int rbo;
    unsigned int screenRbo;
    unsigned int depthAttachmentTexture;
    unsigned int colorAttachmentTexture;
    unsigned int screenColorAttachmentTexture;
    unsigned int screenFbo;

    std::vector<std::shared_ptr<Model>> models;
    int selected_renderer = 0;

    // settings
    unsigned int SCR_WIDTH;
    unsigned int SCR_HEIGHT;
    bool leftMouse = false;
    bool rightMouse = false;
    bool dblClick = false;
    std::chrono::steady_clock::time_point lastClick;

    glm::vec2 mousePos;
    glm::vec2 lastMousePos;
    glm::vec2 lastMousePos2;
    int cursor_radius = 1;
    glm::vec2 scrollDelta;
    int cull_mode = GL_BACK;
    // TODO make private
    bool isLightEnabled = true;
    bool isClippingEnabled = false;
    float meshSize = 0.01f;
    float meshShrink = 0.f;

    unsigned int colormaps[2];
    unsigned int colormaps2D[2];

    // std::vector<std::pair<Element, std::string>> attrs;
    // int selectedAttr = 0;

    glm::mat4 projection;


    void processInput(GLFWwindow *window);


    void run();

    // Frame functions
    int getWidth() final override;
    int getHeight() final override;

    // Utils functions
    void load_model(const std::string& filename) final override;
    void screenshot(const std::string& filename) final override;
    void quit();
    float get_depth(double x, double y);
    unsigned char get_depth2(double x, double y);

    void unproject(int x, int y, float depth, glm::vec3 &p);
    glm::vec3 pick_point(double x, double y);

    long pick(double xPos, double yPos);

	long pick_edge(Volume &m, glm::vec3 p0, int c) final override {
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
    long pick_edge() final override {
        if (st.cell.is_hovered()) {
            auto p = pick_point(mousePos.x, mousePos.y);
            // TODO get current model
            return pick_edge(hex, p, st.cell.get_hovered());
        } else {
            return -1;
        }
    }

    // Pick id of facet of the current rendering
    long pick_facet() final override;
    // Pick id of cell of the current rendering
    long pick_cell() final override;

    std::vector<long> pick(GLFWwindow *window, double xPos, double yPos, int radius);


    // Camera functions
    void reset_zoom() final override;
    void look_at_center() final override;

    // Rendering functions
    bool getLight() final override { return isLightEnabled; }
    void setLight(bool enabled) final override;
    void setClipping(bool enabled) final override;
    void setCullMode(int mode) final override { cull_mode = mode; }

    // Accessors

    std::vector<std::shared_ptr<Model>>& getModels() final override {
        return models;
    }

    int countModels() final override {
        return models.size();
    }

    void setSelectedModel(int selected) final override {
        selected_renderer = selected;
    }

    int getSelectedModel() final override {
        return selected_renderer;
    }

    Model& getCurrentModel() final override {
        return *models[selected_renderer];
    }

    ArcBallCamera& getCamera() final override { return *camera; }
    InputState& getInputState() final override { return st; }

    std::vector<std::string> getPickModeStrings() const { return std::vector<std::string>(pickModeStrings, pickModeStrings + 4); }
    int getPickMode() final override { return pickMode; }
    void setPickMode(Element mode) final override { pickMode = mode; }
    
    std::vector<unsigned int> getColorMaps2D() final override {
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


    Hexahedra hex;

    Element pickMode = Element::CELLS;

    std::vector<int> _lastHovered;


    // Current pressed mouse button, -1 if none
    int _cur_button = -1;

    protected:
    Args args;

    GLFWwindow* window;
    
    std::vector<std::unique_ptr<Component>> scripts;
	InputState st;

    int _dbl_click_interval = 300 /*ms*/;

    private:


};