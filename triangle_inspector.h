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

	// Get 2D screen pos of all points of the model
    std::vector<glm::vec2> getPoint2D(Camera &c, TriModel &model) const {
        std::vector<glm::vec2> points2D;
		points2D.reserve(model.nverts());
		
        for (int i = 0; i < model.nverts(); ++i) {
			vec3 p3 = model.getTriangles().points[i];
			glm::vec4 p(p3.x, p3.y, p3.z, 1.f);
            
			p = c.getProjectionMatrix() * c.getViewMatrix() * p;
			// auto proj = glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.1f, 100.f);
			// p = proj * c.getViewMatrix() * p;
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
    
	std::array<bool, 3> getOverlap(int epsScreen, Surface::Facet &f, const std::vector<glm::vec2> &points2D) {
		std::array<bool, 3> isOverlap = {false};
		// Get 3D pos
		auto v0 = f.vertex(0);
		auto v1 = f.vertex(1);
		auto v2 = f.vertex(2);

		auto pScreen0 = sl::glm2um(points2D[v0]);
		auto pScreen1 = sl::glm2um(points2D[v1]);
		auto pScreen2 = sl::glm2um(points2D[v2]);

		// Check for points
		double p0p1ScreenDist = (pScreen0 - pScreen1).norm(); 
		double p0p2ScreenDist = (pScreen0 - pScreen2).norm(); 
		double p1p2ScreenDist = (pScreen1 - pScreen2).norm();

		// Check for overlap in screen space
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

		return isOverlap;
	}

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

		if (ImGui::Button("View degenerated triangles")) {
			triModel.getPoints().setHoverColor(glm::vec3(1.f, 1.f, 1.f));
			triModel.getPoints().setSelectColor(glm::vec3(0.88f, 0.06f, 0.01f));

			triModel.getMesh().setHoverColor(glm::vec3(1.f, 1.f, 1.f));
			triModel.getMesh().setSelectColor(glm::vec3(1.f, 0.06f, 0.51f));

			triModel.getMesh().setColor(glm::vec3(1.f, 1.f, 1.f));
			triModel.getMesh().setMeshSize(2.f);
		}
		
		// Beuurk !
		if (!once) {
			once = true;
		}

		std::vector<glm::vec2> points2D = getPoint2D(app.getCamera(), triModel);

		// Eps for min distance between two points, to recognized a triangle as geometrically degenerated
		static int eps_digits = 3;
		ImGui::SliderInt("Epsilon digits", &eps_digits, 1, 15);
		eps = 1. / pow(10, eps_digits);
		ImGui::Text("Epsilon: %.15g", eps);

		int epsScreen = triModel.getPoints().getPointSize();
		ImGui::Text("Vertice overlap threshold distance (in pixel): %i", epsScreen);

		// Highlight degenerated points
		PointAttribute<float> pointHl;
		pointHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
		pointHl.fill(0.f);

		// Highlight degenerated facets
		FacetAttribute<float> facetHl;
		facetHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
		facetHl.fill(0.f);

		for (auto &f : m.iter_facets()) {

			bool isOverlap[3] = {false};
			// Get 3D pos
			auto v0 = f.vertex(0);
			auto v1 = f.vertex(1);
			auto v2 = f.vertex(2);

			auto pScreen0 = sl::glm2um(points2D[v0]);
			auto pScreen1 = sl::glm2um(points2D[v1]);
			auto pScreen2 = sl::glm2um(points2D[v2]);

			// Check for points
			double p0p1ScreenDist = (pScreen0 - pScreen1).norm(); 
			double p0p2ScreenDist = (pScreen0 - pScreen2).norm(); 
			double p1p2ScreenDist = (pScreen1 - pScreen2).norm();

			// Check for overlap in screen space
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

			bool isFacetDegenerated = isOverlap[0] || isOverlap[1] || isOverlap[2];

			// List degenerate facets in the UI
			if (isFacetDegenerated) {

				// // Goto facet
				// if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_geo_degenerated_facet" + std::to_string(f)).c_str())) {
				// 	Triangle3 t = f;
				// 	cameraGoto(sl::um2glm(t.bary_verts()));
				// }
				// ImGui::SameLine();
				// ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is geo degenerated.");
				// ImGui::SameLine();
				// // ImGui::Text("Dist: %.15g", dist);

				// // Draw which corners are degenerated in the UI
				// drawUIDegeneratedCorners(f, isOverlap);

				facetHl[f] = 1.f;
				triModel.setHighlight(ElementKind::FACETS);
			}


			// Highlight overlap points
			for (int i = 0; i < 3; ++i) {
				if (isOverlap[i])
					pointHl[f.vertex(i)] = 1;
			}

			triModel.setHighlight(ElementKind::POINTS);



		}


		// // Get inverted facets
		// for (auto &f : m.iter_facets()) {
		// 	Triangle3 t = f;
		// 	vec3 u = t.v[1] - t.v[0];
		// 	vec3 v = t.v[2] - t.v[0];
		// 	double signed_scalar = (t.normal() * (cross(u, v)));
		// 	if (signed_scalar < 0) {
		// 		// Goto facet
		// 		if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_inverted_facet" + std::to_string(f)).c_str())) {
		// 			cameraGoto(sl::um2glm(t.bary_verts()));
		// 		}
		// 	}
		// }


		ImGui::End();



		//
		static int tooltipOpen = -1;
		static ImVec2 tooltipPos;
		if (app.getInputState().facet.anyHovered() && tooltipOpen == -1) {
			auto f = app.getInputState().facet.getAllHovered().front();
			if (facetHl[f] >= 0.5f && tooltipOpen == -1) {
				ImGui::BeginTooltip();
				ImGui::Text("Degenerated facet %ld", f);
				ImGui::Text("Click to open...");
				ImGui::EndTooltip();

				// if (app.getInputState().mouse.isLeftButton()) {
				// 	tooltipOpen = f;
				// 	tooltipPos = ImGui::GetMousePos();
				// }
			}
		}

		if (tooltipOpen != -1) {
			Surface::Facet facet(triModel.getTriangles(), tooltipOpen);
			auto overlaps = getOverlap(epsScreen, facet, points2D);

			ImGui::SetNextWindowPos(tooltipPos);
			ImGui::Begin("Info");

			ImGui::Text("Degenerated facet %i", tooltipOpen);
			ImGui::Text("Overlap vertices:");
			for (int i = 0; i < 3; ++i) {
				if (overlaps[i]) {
					ImGui::Text("%ld", facet.vertex(i));
				}
			}
			if (ImGui::Button("Close")) {
				tooltipOpen = -1;
			}
			ImGui::End();


		}

		return true;
	}
	

	// bool draw_gui(ImGuiContext* ctx) {
	// 	ImGui::Begin("Triangle diagnostic");

	// 	// Check for degenerated
	// 	// Get when model changed event to not compute everytime !
	// 	if (!app.hasModels()) {
	// 		ImGui::End();
	// 		return true;
	// 	}

	// 	// Get current model and check it's a triangle mesh
	// 	auto &model = app.getCurrentModel();
	// 	if (model.getModelType() != Model::ModelType::TRI) {
	// 		ImGui::End();
	// 		return true;
	// 	}

	// 	auto &triModel = model.as<TriModel>();
	// 	auto &m = triModel.getTriangles();

	// 	if (ImGui::Button("View degenerated triangles")) {
	// 		triModel.getPoints().setHoverColor(glm::vec3(1.f, 1.f, 1.f));
	// 		triModel.getPoints().setSelectColor(glm::vec3(0.88f, 0.06f, 0.01f));

	// 		triModel.getMesh().setHoverColor(glm::vec3(1.f, 1.f, 1.f));
	// 		triModel.getMesh().setSelectColor(glm::vec3(1.f, 0.06f, 0.51f));

	// 		triModel.getMesh().setColor(glm::vec3(1.f, 1.f, 1.f));
	// 		triModel.getMesh().setMeshSize(2.f);
	// 	}
		
	// 	// Beuurk !
	// 	if (!once) {
	// 		once = true;
	// 	}

	// 	std::vector<glm::vec2> points2D = getPoint2D(app.getCamera(), triModel);

	// 	// Eps for min distance between two points, to recognized a triangle as geometrically degenerated
	// 	static int eps_digits = 3;
	// 	ImGui::SliderInt("Epsilon digits", &eps_digits, 1, 15);
	// 	eps = 1. / pow(10, eps_digits);
	// 	ImGui::Text("Epsilon: %.15g", eps);

	// 	int epsScreen = triModel.getPoints().getPointSize();
	// 	ImGui::Text("Vertice overlap threshold distance (in pixel): %i", epsScreen);

	// 	// Highlight degenerated points
	// 	PointAttribute<float> pointHl;
	// 	pointHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
	// 	pointHl.fill(0.f);

	// 	// Highlight degenerated facets
	// 	FacetAttribute<float> facetHl;
	// 	facetHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
	// 	facetHl.fill(0.f);

	// 	for (auto &f : m.iter_facets()) {

	// 		bool isOverlap[3] = {false};
	// 		bool isDegenerated[3] = {false};

	// 		// Get 3D pos
	// 		auto v0 = f.vertex(0);
	// 		auto v1 = f.vertex(1);
	// 		auto v2 = f.vertex(2);
	// 		auto p30 = v0.pos();
	// 		auto p31 = v1.pos();
	// 		auto p32 = v2.pos();

	// 		double p0p1Dist = (p30 - p31).norm();
	// 		double p0p2Dist = (p30 - p32).norm();
	// 		double p1p2Dist = (p31 - p32).norm();

	// 		double dist = 0.f;
	// 		if (p0p1Dist < eps && v0 != v1) {
	// 			isDegenerated[0] = true;
	// 			isDegenerated[1] = true;
	// 			dist = p0p1Dist;
	// 		}
	// 		if (p0p2Dist < eps && v0 != v2) {
	// 			isDegenerated[0] = true;
	// 			isDegenerated[2] = true;
	// 			dist = p0p2Dist;
	// 		}
	// 		if (p1p2Dist < eps && v1 != v2) {
	// 			isDegenerated[1] = true;
	// 			isDegenerated[2] = true;
	// 			dist = p1p2Dist;
	// 		}


	// 		auto pScreen0 = sl::glm2um(points2D[v0]);
	// 		auto pScreen1 = sl::glm2um(points2D[v1]);
	// 		auto pScreen2 = sl::glm2um(points2D[v2]);


	// 		// Check for points
	// 		double p0p1ScreenDist = (pScreen0 - pScreen1).norm(); 
	// 		double p0p2ScreenDist = (pScreen0 - pScreen2).norm(); 
	// 		double p1p2ScreenDist = (pScreen1 - pScreen2).norm();



	// 		bool isFacetDegenerated = isDegenerated[0] || isDegenerated[1] || isDegenerated[2];

	// 		// List degenerate facets in the UI
	// 		if (isFacetDegenerated) {

	// 			// Check for overlap in screen space
	// 			if (p0p1ScreenDist < epsScreen) {
	// 				isOverlap[0] = true;
	// 				isOverlap[1] = true;
	// 			}
	// 			if (p0p2ScreenDist < epsScreen) {
	// 				isOverlap[0] = true;
	// 				isOverlap[2] = true;
	// 			}
	// 			if (p1p2ScreenDist < epsScreen) {
	// 				isOverlap[1] = true;
	// 				isOverlap[2] = true;
	// 			}

	// 			// Goto facet
	// 			if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_geo_degenerated_facet" + std::to_string(f)).c_str())) {
	// 				Triangle3 t = f;
	// 				cameraGoto(sl::um2glm(t.bary_verts()));
	// 			}
	// 			ImGui::SameLine();
	// 			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is geo degenerated.");
	// 			ImGui::SameLine();
	// 			ImGui::Text("Dist: %.15g", dist);

	// 			// Draw which corners are degenerated in the UI
	// 			drawUIDegeneratedCorners(f, isDegenerated);

	// 			facetHl[f] = 1.f;
	// 			triModel.setHighlight(ElementKind::FACETS);
	// 		}


	// 		// Highlight overlap points
	// 		for (int i = 0; i < 3; ++i) {
	// 			if (isOverlap[i])
	// 				pointHl[f.vertex(i)] = 1;
	// 		}

	// 		triModel.setHighlight(ElementKind::POINTS);



	// 	}


	// 	// // Get inverted facets
	// 	// for (auto &f : m.iter_facets()) {
	// 	// 	Triangle3 t = f;
	// 	// 	vec3 u = t.v[1] - t.v[0];
	// 	// 	vec3 v = t.v[2] - t.v[0];
	// 	// 	double signed_scalar = (t.normal() * (cross(u, v)));
	// 	// 	if (signed_scalar < 0) {
	// 	// 		// Goto facet
	// 	// 		if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_inverted_facet" + std::to_string(f)).c_str())) {
	// 	// 			cameraGoto(sl::um2glm(t.bary_verts()));
	// 	// 		}
	// 	// 	}
	// 	// }


	// 	ImGui::End();
	// 	return true;
	// }

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

};