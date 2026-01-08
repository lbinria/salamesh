#pragma once 
#include "../core/app_interface.h"
#include "../core/component.h"
#include "../core/element.h"
#include "../core/helpers.h"

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

#include "../../include/json.hpp"
using json = nlohmann::json;


#include "imgui.h"
#include "imgui_internal.h"

struct InvertedTriangleViewer : public Component {

	InvertedTriangleViewer(IApp &app) : app(app) {}


	// Lifecycle
	void init() {

	}

	// virtual void setup() = 0;
	void cleanup() {}

	void modelLoaded(const std::string &path) override {}

	// Check if nav path is "inverted_triangles_view"
	bool isNavHere() {
		return app.getNavigationPathString() == "diagnostic/inverted_triangles_view";
	}


	bool draw_gui(ImGuiContext* ctx) {
		// Check that nav path is "diagnostic"
		if (app.getNavigationPath().size() == 0 || app.getNavigationPath().front() != "diagnostic")
			return true;

		ImGui::Begin("Triangle diagnostic");
		
		bool isNav = isNavHere();

		if (isNav) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("View inverted triangles...")) {
			app.setNavigationPath({"diagnostic", "inverted_triangles_view"});

			// Setup renderers
			// Get when model changed event to not compute everytime !
			if (!app.hasModels())
				return true;

			// Get current model and check it's a triangle mesh
			auto &model = app.getCurrentModel();

			if (model.getModelType() != ModelType::TRI_MODEL)
				return true;

			auto &triModel = model.as<TriModel>();
			triModel.getEdgesRenderer()->setVisible(true);


		}

		if (isNav) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		if (isNav) {
			ImGui::Text("N inverted: %i", nInverted);
		}

		ImGui::End();

		return true;
	}
	
	// inline std::optional<TriModel&> getModel(IApp &app) {
	// 	auto &model = app.getCurrentModel();

	// 	if (model.getModelType() != Model::ModelType::TRI)
	// 		return std::nullopt;

	// 	auto &triModel = model.as<TriModel>();
	// 	return triModel;
	// }

    void update(float dt) {
		
		if (!isNavHere())
			return;

		// Get when model changed event to not compute everytime !
		if (!app.hasModels())
			return;

		// Get current model and check it's a triangle mesh
		auto &model = app.getCurrentModel();

		if (model.getModelType() != ModelType::TRI_MODEL)
			return;

		auto &triModel = model.as<TriModel>();
		auto &m = triModel.getTriangles();

		// Check for inverted triangles
		// auto dir = sl::glm2um(app.getCamera().getViewDir());
		auto camPos = sl::glm2um(app.getCamera().getEye());

		CornerAttribute<float> ca;
		ca.bind("_highlight", triModel.getSurfaceAttributes(), m);
		ca.fill(0.f);

		int n = 0;
		for (auto &f : m.iter_facets()) {
			Triangle3 t = f;

			vec3 d{0,0,0};
			for (int i = 0; i < 3; ++i)
				d += camPos - f.vertex(i).pos();
			d /= 3.;

			if (t.normal() * d < 0) {
				// Something to do
				ca[f.halfedge(0)] = 1.f;
				ca[f.halfedge(1)] = 1.f;
				ca[f.halfedge(2)] = 1.f;
				++n;
			}
		}

		nInverted = n;
		triModel.setHighlight(ElementKind::CORNERS_ELT);

	}

    // Input events
	void mouse_move(double x, double y) {}
	void mouse_button(int button, int action, int mods) {}
    void mouse_scroll(double xoffset, double yoffset) {}
    void key_event(int key, int scancode, int action, int mods) {}

	void navigationPathChanged(const std::vector<std::string> &oldNavPath, const std::vector<std::string> &newNavPath) override {

	}


	private:

	IApp &app;
	int nInverted = 0;

};