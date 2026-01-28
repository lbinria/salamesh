#pragma once 

#include "app.h"
#include "core/lua_script.h"
#include "helpers/args_manager.h"

// JSON !!
#include <json.hpp>
using json = nlohmann::json;

struct MyApp : App {

	MyApp(Args args) : App(args) {}

	// Override lifecycle functions
	void init() override;
	void draw_gui() override;

	// Move that in a layout component
	void setupDock();


};