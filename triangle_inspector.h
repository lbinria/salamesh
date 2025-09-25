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
				auto v0 = f.vertex(0);
				auto v1 = f.vertex(1);
				auto v2 = f.vertex(2);

				bool topo_degenerated[3] = {false};

				if (v0 == v1) {
					topo_degenerated[0] = true;
					topo_degenerated[1] = true;
				}
				if (v0 == v2) {
					topo_degenerated[0] = true;
					topo_degenerated[2] = true;
				}
				if (v1 == v2) {
					topo_degenerated[1] = true;
					topo_degenerated[2] = true;
				}

				bool isTopoDegenerated = topo_degenerated[0] || topo_degenerated[1] || topo_degenerated[2];
				if (isTopoDegenerated) {

					if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_topo_degenerated_facet" + std::to_string(f)).c_str())) {
						Triangle3 t = f;
						vec3 fPos = t.bary_verts();
						glm::vec3 glmFPos(fPos.x, fPos.y, fPos.z);
						auto dir = glm::normalize(app.getCamera().getEye() - glmFPos);
						glm::vec3 newPos = glmFPos - dir * 1.f; 
						app.getCamera().setEye(newPos);
						app.getCamera().lookAt(glmFPos);
					}
					ImGui::SameLine();

					ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is topo degenerated. Following corners coincide: ");

					for (int i = 0; i < 3; ++i) {
						if (topo_degenerated[i]) {

							std::string lnkLabel = "corner " 
								+ std::to_string(i) 
								+ " (vert " 
								+ std::to_string(f.vertex(i)) 
								+ ")##link_goto_topo_degenerated_vertex" 
								+ std::to_string(f.vertex(i));

							ImGui::SameLine();
							if (ImGui::TextLink(lnkLabel.c_str())) {
								// Compute line between current camera pos and vertex pos
								vec3 vPos = f.vertex(i).pos();
								glm::vec3 glmVPos(vPos.x, vPos.y, vPos.z);
								auto dir = glm::normalize(app.getCamera().getEye() - glmVPos);
								glm::vec3 newPos = glmVPos - dir * 1.f; 
								app.getCamera().setEye(newPos);
								app.getCamera().lookAt(glmVPos);
							}
						}
					}

				}

				auto p0 = f.vertex(0).pos();
				auto p1 = f.vertex(1).pos();
				auto p2 = f.vertex(2).pos();

				// Check for points
				float p0p1Dist = (p0 - p1).norm2(); 
				float p0p2Dist = (p0 - p2).norm2(); 
				float p1p2Dist = (p1 - p2).norm2();

				if (!isTopoDegenerated && (p0p1Dist < eps || p0p2Dist < eps || p1p2Dist < eps)) {
					if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_geo_degenerated_facet" + std::to_string(f)).c_str())) {
						Triangle3 t = f;
						vec3 fPos = t.bary_verts();
						glm::vec3 glmFPos(fPos.x, fPos.y, fPos.z);
						auto dir = glm::normalize(app.getCamera().getEye() - glmFPos);
						glm::vec3 newPos = glmFPos - dir * 1.f; 
						app.getCamera().setEye(newPos);
						app.getCamera().lookAt(glmFPos);
					}
					ImGui::SameLine();

					ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is geo degenerated.");

					if (ImGui::TextLink(("Hide others##link_show_only_geo_degenerated_facet" + std::to_string(f)).c_str())) {
						FacetAttribute<float> filter;
						filter.bind("_filter", tri_model.getSurfaceAttributes(), m);

						for (auto &cf : m.iter_facets()) {
							if (cf != f) {
								filter[cf] = 1.f;
							}
						}
						model.setFilter(ElementKind::FACETS);
					}
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