#pragma once 

#include "app.h"
#include "lua_script.h"
#include "helpers/args_manager.h"

// JSON !!
#include <json.hpp>
using json = nlohmann::json;

struct MyApp : App {

	MyApp(Args args) : App(args) {}


	void updateCamera(float dt);

	// Override lifecycle functions
	void init() override;
	void update(float dt) override;
	void draw_gui() override;

	// Override event functions
    void key_event(int key, int scancode, int action, int mods) override;
	void mouse_scroll(double xoffset, double yoffset) override;
	void mouse_button(int button, int action, int mods) override;
	void mouse_move(double x, double y) override;



	virtual std::unique_ptr<Camera> makeCamera(std::string type) override;

	// Move that in a layout component
	void TopModePanel(int &currentMode, const std::vector<std::pair<std::string, ImTextureID>>& icons, ImVec2 iconSize = ImVec2(28,28));

	private:

	bool isUIHovered = false;

};