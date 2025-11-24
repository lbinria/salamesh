#pragma once 

#include "imgui.h"

struct Component {

    const std::string id() const {
        static thread_local unsigned long long counter = 0;
        static auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::stringstream ss;
        ss << "component-" << now << "-" << counter++;
        return ss.str();
    }

    // Lifecycle
    virtual void init() = 0;
    // virtual void setup() = 0;
	virtual void cleanup() = 0;
    virtual bool draw_gui(ImGuiContext* ctx) = 0;
    virtual void update(float dt) = 0;

    // Input events
	virtual void mouse_move(double x, double y) = 0;
	virtual void mouse_button(int button, int action, int mods) = 0;
    virtual void mouse_scroll(double xoffset, double yoffset) = 0;
    virtual void key_event(int key, int scancode, int action, int mods) = 0;

    // App events
    // TODO rename to onNavigationPathChange
    virtual void navigationPathChanged(const std::vector<std::string> &oldNavPath, const std::vector<std::string> &newNavPath) {};
    // TODO rename to onModelLoad
    virtual void modelLoaded(const std::string &path) {};
    virtual void onSelectedModelChange(int idx) {};
    
};