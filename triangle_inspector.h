#pragma once 
#include "core/component.h"

#include <string>

struct TriangleInspector : public Component {

	TriangleInspector(IApp &app) : app(app) {}

    // Lifecycle
    void init() {}
    // virtual void setup() = 0;
	void cleanup() {}
    
	bool draw_gui(ImGuiContext* ctx) {
		ImGui::Begin("Triangle inspector");


		// Check for degenerated
		// Get when model changed event to not compute everytime !
		if (app.hasModels()) {

			auto &model = app.getCurrentModel();
			if (model.getModelType() != Model::ModelType::TRI)
				return true;

			auto &tri_model = model.as<TriModel>();
			auto &m = tri_model.getTriangles();
			
			// ImGui::SliderFloat("Distance epsilon", &eps, 0.00000001f, 0.1f);
			static int eps_digits = 3;
			ImGui::SliderInt("Epsilon digits", &eps_digits, 1, 15);
			eps = 1. / pow(10, eps_digits);
			ImGui::Text("Epsilon: %.15g", eps);

			// static float logv = -9.0f; // log10(eps)
			// if (ImGui::SliderFloat("log10(eps)", &logv, -12.0f, -6.0f, "%.2f")) {
			// 	// changed
			// }
			// float eps = powf(10.0f, logv);

			for (auto &f : m.iter_facets()) {
				auto p0 = f.vertex(0).pos();
				auto p1 = f.vertex(1).pos();
				auto p2 = f.vertex(2).pos();

				// Check for points
				float p0p1Dist = (p0 - p1).norm2(); 
				float p0p2Dist = (p0 - p2).norm2(); 
				float p1p2Dist = (p1 - p2).norm2();

				if (p0p1Dist < eps || p0p2Dist < eps || p1p2Dist < eps) {
					std::string s = "Facet " + std::to_string(f) + " is geo degenerated.";
					ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), s.c_str());
				}
			}

		}

		ImGui::End();
		return true;
	}
    void update(float dt) {}

    // Input events
	void mouse_move(double x, double y) {}
	void mouse_button(int button, int action, int mods) {}
    void mouse_scroll(double xoffset, double yoffset) {}
    void key_event(int key, int scancode, int action, int mods) {}

    // App events
    void componentChanged(const std::string &id) {}

	private:
	IApp &app;
	double eps = 0.001;

};