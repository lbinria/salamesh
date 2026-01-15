#pragma once 
#include "../core/app_interface.h"
#include "../core/component.h"
#include "../core/helpers.h"

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

#include "../../include/json.hpp"
using json = nlohmann::json;


struct TriangleDiagnosticLayout : public Script {

	TriangleDiagnosticLayout(IApp &app) : Script(app) {}


	// Lifecycle
	void init() {

	}

	// virtual void setup() = 0;
	void cleanup() {}

	void modelLoaded(const std::string &path) override {}
    



	bool draw_gui(ImGuiContext* ctx) {
		// Check that nav path is "diagnostic"
		if (app.getNavigationPath().size() == 0 || app.getNavigationPath().front() != "diagnostic")
			return true;


		ImGui::Begin("Triangle diagnostic");
		
		ImGui::End();

		return true;
	}
	


    void update(float dt) {}

    // Input events
	void mouse_move(double x, double y) {}
	void mouse_button(int button, int action, int mods) {}
    void mouse_scroll(double xoffset, double yoffset) {}
    void key_event(int key, int scancode, int action, int mods) {}

	void navigationPathChanged(const std::vector<std::string> &oldNavPath, const std::vector<std::string> &newNavPath) override {

	}

};