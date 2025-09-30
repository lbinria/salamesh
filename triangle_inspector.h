#pragma once 
#include "core/component.h"
#include "core/helpers.h"

#include <string>
#include <random>
#include <sstream>
#include <iomanip>
struct TriangleInspector : public Component {

	TriangleInspector(IApp &app) : app(app) {}

	void cameraGoto(glm::vec3 p) {
		auto dir = glm::normalize(app.getCamera().getEye() - p);
		glm::vec3 newPos = p - dir * 1.f; 
		app.getCamera().setEye(newPos);
		app.getCamera().lookAt(p);
	}

	void drawUIDegeneratedCorners(Surface::Facet &f, bool degenerated[3]) {
		for (int i = 0; i < 3; ++i) {

			if (!degenerated[i]) continue;

			std::string lnkLabel = "corner " 
				+ std::to_string(i) 
				+ " (vert " 
				+ std::to_string(f.vertex(i)) 
				+ ")##link_goto_vert_"
				+ std::to_string(f.vertex(i));

			ImGui::SameLine();
			if (ImGui::TextLink(lnkLabel.c_str())) {
				cameraGoto(sl::um2glm(f.vertex(i).pos()));
			}
		}
	}

    std::vector<glm::vec2> getPoint2D(Camera &c, TriModel &model) const {
        std::vector<glm::vec2> points2D;
        for (int i = 0; i < model.nverts(); ++i) {
			vec3 p3 = model.getTriangles().points[i];
			glm::vec4 p(p3.x, p3.y, p3.z, 1.f);
            
			p = c.getProjectionMatrix() * c.getViewMatrix() * p;
            p /= p.w;
			
			p.x = (p.x + 1.f) * 0.5f * app.getScreenWidth();
			p.y = (1.f - (p.y + 1.f) * 0.5f) * app.getScreenHeight();

            points2D.push_back(glm::vec2(p.x, p.y));
        }
        return points2D;	
    }

	

    // Lifecycle
    void init() {

	}

    // virtual void setup() = 0;
	void cleanup() {}

	void modelLoaded(const std::string &path) override {}
    
	

	bool draw_gui(ImGuiContext* ctx) {
		ImGui::Begin("Triangle diagnostic");

		// Check for degenerated
		// Get when model changed event to not compute everytime !
		if (!app.hasModels()) {
			ImGui::End();
			return true;
		}

		// Get current model and check it's a triangle mesh
		auto &model = app.getCurrentModel();
		if (model.getModelType() != Model::ModelType::TRI) {
			ImGui::End();
			return true;
		}

		auto &triModel = model.as<TriModel>();
		auto &m = triModel.getTriangles();
		
		// Beuurk !
		if (!once) {
			triModel.getPoints().setHoverColor(glm::vec3(1.f, 1.f, 1.f));
			triModel.getPoints().setSelectColor(glm::vec3(0.88f, 0.06f, 0.01f));
			once = true;
		}

		std::vector<glm::vec2> points2D = getPoint2D(app.getCamera(), triModel);

		// Eps for min distance square between two points, to be recognized a triangle as geometrically degenerated
		static int eps_digits = 3;
		ImGui::SliderInt("Epsilon digits", &eps_digits, 1, 15);
		eps = 1. / pow(10, eps_digits);
		ImGui::Text("Epsilon: %.15g", eps);

		
		// Highlight degenerated points
		PointAttribute<float> pointHl;
		pointHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
		pointHl.fill(0.f);

		for (auto &f : m.iter_facets()) {

			bool isOverlap[3] = {false};
			bool isDegenerated[3] = {false};

			// Get 3D pos
			auto p30 = f.vertex(0).pos();
			auto p31 = f.vertex(1).pos();
			auto p32 = f.vertex(2).pos();

			double p0p1Dist = (p30 - p31).norm();
			double p0p2Dist = (p30 - p32).norm();
			double p1p2Dist = (p31 - p32).norm();

			if (p0p1Dist < eps) {
				isDegenerated[0] = true;
				isDegenerated[1] = true;
			}
			if (p0p2Dist < eps) {
				isDegenerated[0] = true;
				isDegenerated[2] = true;
			}
			if (p1p2Dist < eps) {
				isDegenerated[1] = true;
				isDegenerated[2] = true;
			}


			auto pScreen0 = sl::glm2um(points2D[f.vertex(0)]);
			auto pScreen1 = sl::glm2um(points2D[f.vertex(1)]);
			auto pScreen2 = sl::glm2um(points2D[f.vertex(2)]);


			// Check for points
			double p0p1ScreenDist = (pScreen0 - pScreen1).norm(); 
			double p0p2ScreenDist = (pScreen0 - pScreen2).norm(); 
			double p1p2ScreenDist = (pScreen1 - pScreen2).norm();

			if (p0p1ScreenDist < epsScreen) {
				isOverlap[0] = true;
				isOverlap[1] = true;
			}
			if (p0p2ScreenDist < epsScreen) {
				isOverlap[0] = true;
				isOverlap[2] = true;
			}
			if (p1p2ScreenDist < epsScreen) {
				isOverlap[1] = true;
				isOverlap[2] = true;
			}

			bool isFacetDegenerated = isDegenerated[0] || isDegenerated[1] || isDegenerated[2];

			// List degenerate facets in the UI
			if (isFacetDegenerated) {

				// Goto facet
				if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_geo_degenerated_facet" + std::to_string(f)).c_str())) {
					Triangle3 t = f;
					cameraGoto(sl::um2glm(t.bary_verts()));
				}
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is geo degenerated.");

				// Draw which corners are degenerated in the UI
				drawUIDegeneratedCorners(f, isDegenerated);
			}


			// Highlight overlap points
			for (int i = 0; i < 3; ++i) {
				if (isOverlap[i])
					pointHl[f.vertex(i)] = 1;
			}

			triModel.setHighlight(ElementKind::POINTS);
		}


		// for (auto &f : m.iter_facets()) {
		// 	auto v0 = f.vertex(0);
		// 	auto v1 = f.vertex(1);
		// 	auto v2 = f.vertex(2);

		// 	bool topo_degenerated[3] = {false};

		// 	if (v0 == v1) {
		// 		topo_degenerated[0] = true;
		// 		topo_degenerated[1] = true;
		// 	}
		// 	if (v0 == v2) {
		// 		topo_degenerated[0] = true;
		// 		topo_degenerated[2] = true;
		// 	}
		// 	if (v1 == v2) {
		// 		topo_degenerated[1] = true;
		// 		topo_degenerated[2] = true;
		// 	}

		// 	bool isTopoDegenerated = topo_degenerated[0] || topo_degenerated[1] || topo_degenerated[2];
		// 	if (isTopoDegenerated) {

		// 		if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_topo_degenerated_facet_" + std::to_string(f)).c_str())) {
		// 			Triangle3 t = f;
		// 			cameraGoto(sl::um2glm(t.bary_verts()));
		// 		}
		// 		ImGui::SameLine();

		// 		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is topo degenerated. Following corners coincide: ");

		// 		drawUIDegeneratedCorners(f, topo_degenerated);
		// 	}

		// 	bool geo_degenerated[3] = {false};

		// 	// auto p0 = f.vertex(0).pos();
		// 	// auto p1 = f.vertex(1).pos();
		// 	// auto p2 = f.vertex(2).pos();
		// 	auto gp0 = points2D[f.vertex(0)];
		// 	auto gp1 = points2D[f.vertex(1)];
		// 	auto gp2 = points2D[f.vertex(2)];
		// 	auto p0 = vec3(gp0.x, gp0.y, gp0.z);
		// 	auto p1 = vec3(gp1.x, gp1.y, gp1.z);
		// 	auto p2 = vec3(gp2.x, gp2.y, gp2.z);

		// 	eps = 2.0f;

		// 	// Check for points
		// 	double p0p1Dist = (p0 - p1).norm(); 
		// 	double p0p2Dist = (p0 - p2).norm(); 
		// 	double p1p2Dist = (p1 - p2).norm();

		// 	if (p0p1Dist < eps) {
		// 		geo_degenerated[0] = true;
		// 		geo_degenerated[1] = true;
		// 	}
		// 	if (p0p2Dist < eps) {
		// 		geo_degenerated[0] = true;
		// 		geo_degenerated[2] = true;
		// 	}
		// 	if (p1p2Dist < eps) {
		// 		geo_degenerated[1] = true;
		// 		geo_degenerated[2] = true;
		// 	}

		// 	bool isGeoDegenerated = !isTopoDegenerated && (geo_degenerated[0] || geo_degenerated[1] || geo_degenerated[2]);



		// 	for (int i = 0; i < 3; ++i) {
		// 		if ((!isTopoDegenerated && geo_degenerated[i]))
		// 			pointHl[f.vertex(i)] = 1;
		// 		// pointHl[f.vertex(i)] = (!isTopoDegenerated && geo_degenerated[i]) ? 1 : 0;
		// 	}
		// 	tri_model.setHighlight(ElementKind::POINTS);



		// 	if (isGeoDegenerated) {

		// 		// Goto facet
		// 		if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_geo_degenerated_facet" + std::to_string(f)).c_str())) {
		// 			Triangle3 t = f;
		// 			cameraGoto(sl::um2glm(t.bary_verts()));
		// 		}
		// 		ImGui::SameLine();
		// 		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is geo degenerated.");

		// 		drawUIDegeneratedCorners(f, geo_degenerated);
		// 	}
		// }

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

	bool once = false;
	double eps = 0.001;
	int epsScreen = 2;

};