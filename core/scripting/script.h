#pragma once 

#include "imgui.h"

struct Script {

	Script(IApp &app) : app(app) {}

	const std::string id() const {
		static thread_local unsigned long long counter = 0;
		static auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::stringstream ss;
		ss << "script-" << now << "-" << counter++;
		return ss.str();
	}

	// Lifecycle
	virtual void init() {};
	virtual void cleanup() {};
	virtual bool drawGui(ImGuiContext* ctx) { return true; };
	virtual void update(float dt) {};
	virtual std::vector<std::pair<std::string, std::string>> layoutGui() { return {}; }

	// Input events
	virtual void mouseMove(double x, double y) {};
	virtual void mouseButton(int button, int action, int mods) {};
	virtual void mouseScroll(double xoffset, double yoffset) {};
	virtual void keyEvent(int key, int scancode, int action, int mods) {};

	// App events
	virtual void navigationPathChanged(NavigationPath &oldNavPath, NavigationPath &newNavPath) {};
	
	virtual void modelLoaded(const std::string &name) {};
	virtual void selectedModelChanged(std::string oldName, std::string newName) {};
	virtual void sceneCleared() {};

	enum Status {
		SCRIPT_STATUS_OK,
		SCRIPT_STATUS_FAILED
	};

	Status status = SCRIPT_STATUS_OK;

	protected:
	IApp &app;
};