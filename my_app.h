#pragma once 

#include "app.h"
#include "core/lua_script.h"
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

	virtual std::unique_ptr<Camera> makeCamera(std::string type) override;

	// Move that in a layout component
	void setupDock();
	void TopModePanel(int &currentMode, const std::vector<std::pair<std::string, ImTextureID>>& icons, ImVec2 iconSize = ImVec2(28,28));

	private:


};