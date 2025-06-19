#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/app_interface.h"

#include "imgui.h"
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

#include "tri_mesh.h"
#include "hex_renderer.h"
#include "points_cloud.h"
#include "wireframe.h"

#include "core/arcball_camera.h"

#include "tool.h"
#include "script.h"

#include <ultimaille/all.h>



using namespace UM;

// void mouse_callback(GLFWwindow* window, double xpos, double ypos);

struct App : public IApp {


    enum ColorMode {
        COLOR,
        ATTRIBUTE,
    };

    const float FAR_PLANE = 100.f;
    const float NEAR_PLANE = 0.1f;

    App() : 
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

    // settings
    std::unique_ptr<HexRenderer> hex_renderer;
    // TODO 20250516 objective remove ptr above !
    // HexRenderer mesh;

    std::unique_ptr<PointsCloud> mesh_ps;
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

    int selectedColormap = 0;
    unsigned int colormaps[2];
    unsigned int colormaps2D[2];

    std::vector<std::pair<Element, std::string>> attrs;
    int selectedAttr = 0;

    glm::mat4 projection;


    void processInput(GLFWwindow *window);


    void run();

    // Frame functions
    int getWidth() final override;
    int getHeight() final override;

    // Utils functions
    void load_model(const std::string& filename);
    void screenshot(const std::string& filename);
    void quit();
    float get_depth(double x, double y);
    unsigned char get_depth2(double x, double y);

    void unproject(int x, int y, float depth, glm::vec3 &p);
    glm::vec3 pick_point(double x, double y);

    long pick(double xPos, double yPos);
    long pick_cell();

    std::vector<long> pick(GLFWwindow *window, double xPos, double yPos, int radius);


    // Camera functions
    void reset_zoom() final override;
    void look_at_center() final override;

    // Rendering functions
    bool getLight() final override { return isLightEnabled; }
    void setLight(bool enabled) final override;
    void setClipping(bool enabled) final override;
    void setCullMode(int mode) final override { cull_mode = mode; }

    Renderer& getRenderer() final override { return *hex_renderer; }
    ArcBallCamera& getCamera() final override { return *camera; }

    std::vector<std::string> getPickModeStrings() const { return std::vector<std::string>(pickModeStrings, pickModeStrings + 4); }
    int getPickMode() { return pickMode; }
    void setPickMode(Element mode) { pickMode = mode; }

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
    const char* colorModeStrings[2] = {"Color", "Attribute"};


    Hexahedra hex;
    VolumeAttributes attributes;

    Element pickMode = Element::CELLS;
    ColorMode colorMode = COLOR;

    std::vector<int> _lastHovered;


    // Current pressed mouse button, -1 if none
    int _cur_button = -1;

    protected:
    int _dbl_click_interval = 300 /*ms*/;
    GLFWwindow* window;
    
    std::vector<std::unique_ptr<Component>> scripts;

    private:

};