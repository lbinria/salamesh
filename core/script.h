#pragma once 

#include "imgui.h"

struct Script {

	Script(IApp &app) : app(app) {}

	const std::string id() const {
		static thread_local unsigned long long counter = 0;
		static auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::stringstream ss;
		ss << "component-" << now << "-" << counter++;
		return ss.str();
	}

	// Lifecycle
	virtual void init() {};
	virtual void cleanup() {};
	virtual bool drawGui(ImGuiContext* ctx) { return true; };
	virtual void update(float dt) {};
	virtual std::vector<std::pair<std::string, std::string>> layoutGui() { return {}; }

	// Input events
	virtual void mouse_move(double x, double y) {};
	virtual void mouse_button(int button, int action, int mods) {};
	virtual void mouse_scroll(double xoffset, double yoffset) {};
	virtual void key_event(int key, int scancode, int action, int mods) {};

	// App events
	virtual void navigationPathChanged(const std::vector<std::string> &oldNavPath, const std::vector<std::string> &newNavPath) {};
	
	virtual void modelLoaded(const std::string &name) {};
	virtual void selectedModelChanged(std::string oldName, std::string newName) {};
	virtual void sceneCleared() {};

	protected:
	IApp &app;
};