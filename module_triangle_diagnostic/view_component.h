#pragma once 
#include "../core/app_interface.h"
#include "../core/script.h"
#include "../core/helpers.h"

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

#include "../../include/json.hpp"
using json = nlohmann::json;


struct ViewComponent : public Script {

	ViewComponent(IApp &app) : Script(app) {}


	// Lifecycle
	void init() {

	}

	// virtual void setup() = 0;
	void cleanup() {}

	void modelLoaded(const std::string &path) override {}
    



	bool drawGui(ImGuiContext* ctx) {
		return true;
	}
	


    void update(float dt) {}

    // Input events
	void mouse_move(double x, double y) {}
	void mouse_button(int button, int action, int mods) {}
    void mouse_scroll(double xoffset, double yoffset) {}
    void key_event(int key, int scancode, int action, int mods) {}

	void navigationPathChanged(const std::vector<std::string> &oldNavPath, const std::vector<std::string> &newNavPath) override {
		// When exit view component
		if (oldNavPath.size() > 0 && oldNavPath.front() == "view" && newNavPath.size() > 0 && newNavPath.front() != "view") {
			// Save gfx states of renderers
			rendererStates.clear();

			auto models = app.getModels();
			for (auto &m : models) {
				auto modelName = m->getName();
				for (auto &[rn, r] : m->getRenderers()) {
					json j;
					r->saveState(j);
					rendererStates[{modelName, rn}] = j;
				}
			}

		// When enter view component
		} else if (oldNavPath.size() > 0 && oldNavPath.front() != "view" && newNavPath.size() > 0 && newNavPath.front() == "view") {
			// Restore gfx states of renderers
			for (auto &[modelAndRendererName, state] : rendererStates) {
				auto &[modelName, rendererName] = modelAndRendererName;

				auto model = app.getModel(modelName);

				auto renderers = model->getRenderers();
				if (renderers.contains(rendererName)) {
					renderers[rendererName]->loadState(state);
				}

				model->unsetFilters();
				model->unsetHighlights();
			}
		}
	}


	private:
	std::map<std::pair<std::string, std::string>, json> rendererStates;

};