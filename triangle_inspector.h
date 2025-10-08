#pragma once 
#include "core/component.h"
#include "core/helpers.h"
#include "core/line_renderer.h"

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
			p /= p.w;
			
			p.x = (p.x + 1.f) * 0.5f * app.getScreenWidth();
			p.y = (1.f - (p.y + 1.f) * 0.5f) * app.getScreenHeight();

			points2D.push_back(glm::vec2(p.x, p.y));
		}
		return points2D;	
	}

	

	// Lifecycle
	void init() {
		lineRenderer.init();
	}

	// virtual void setup() = 0;
	void cleanup() {
		lineRenderer.clean();
	}

	void modelLoaded(const std::string &path) override {}



	float getRadius(glm::mat4 &pvm, glm::vec3 pos, float pointSize, glm::vec2 &viewport) {
		// Clip space
		glm::vec4 p = pvm * glm::vec4(pos, 1.0);
		// Compute 2D pos in pixel of center of point
		glm::vec4 ndc = p / p.w;
		glm::vec2 screenPos = (glm::vec2{ndc.x, ndc.y} * 0.5f + 0.5f) * viewport;
		// Move point by adding pixel offset
		// screenPos += glm::vec2{static_cast<float>(pointSize), static_cast<float>(pointSize) } * .5f;
		screenPos += glm::vec2{static_cast<float>(pointSize * .5f), 0.f };
		// Back to the future... heu NDC
		glm::vec2 ndc2 = screenPos / viewport * 2.f - 1.f;
		float clipW = p.w;
		float clipZ = p.z;
		// Inverse of perspective division, return to clip space
		glm::vec4 clipNPos = glm::vec4(ndc2 * clipW, clipZ, clipW);
		// Back to world space
		glm::vec4 worldNPos = glm::inverse(pvm) * clipNPos;
		// Compute dist between offset point and starting point
		return glm::length(glm::vec3{worldNPos} - pos);
	}

	// std::array<bool, 3> getOverlap(Camera &c, glm::mat4 modelMat, int epsScreen, Surface::Facet &f, const std::vector<glm::vec2> &points2D) {
	// 	std::array<bool, 3> isOverlap = {false};
	// 	// Get 3D pos
	// 	auto v0 = f.vertex(0);
	// 	auto v1 = f.vertex(1);
	// 	auto v2 = f.vertex(2);

	// 	glm::vec3 p0 = sl::um2glm(v0.pos());
	// 	glm::vec3 p1 = sl::um2glm(v1.pos());
	// 	glm::vec3 p2 = sl::um2glm(v2.pos());

	// 	auto pScreen0 = sl::glm2um(points2D[v0]);
	// 	auto pScreen1 = sl::glm2um(points2D[v1]);
	// 	auto pScreen2 = sl::glm2um(points2D[v2]);

	// 	// Check for points
	// 	double p0p1ScreenDist = (pScreen0 - pScreen1).norm(); 
	// 	double p0p2ScreenDist = (pScreen0 - pScreen2).norm(); 
	// 	double p1p2ScreenDist = (pScreen1 - pScreen2).norm();

	// 	float zDist = 0.f;
	// 	if (p0p1ScreenDist < epsScreen || 
	// 		p0p2ScreenDist < epsScreen ||
	// 		p1p2ScreenDist < epsScreen) {
	// 			// Only compute Z here
	// 			zDist = getEps(c, modelMat, sl::um2glm(v0.pos()), epsScreen);
	// 	}

	// 	// Check for overlap in screen space
	// 	if (p0p1ScreenDist < epsScreen && abs(p0.z - p1.z) < zDist) {
	// 		isOverlap[0] = true;
	// 		isOverlap[1] = true;
	// 	}
	// 	if (p0p2ScreenDist < epsScreen && abs(p0.z - p2.z) < zDist) {
	// 		isOverlap[0] = true;
	// 		isOverlap[2] = true;
	// 	}
	// 	if (p1p2ScreenDist < epsScreen && abs(p1.z - p2.z) < zDist) {
	// 		isOverlap[1] = true;
	// 		isOverlap[2] = true;
	// 	}

	// 	return isOverlap;
	// }

	bool draw_gui(ImGuiContext* ctx) {
		// Check that nav path is "diagnostic"
		if (app.getNavigationPath().size() == 0 || app.getNavigationPath().front() != "diagnostic") {
			return true;
		}

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
			
			if (app.getNavigationPath().front() == "diagnostic" && app.getNavigationPath().back() != "degenerated triangles") {
				// app.addNavigationPath("degenerated triangles");

				// Setup gfx
				triModel.getPoints().setHoverColor(glm::vec3(1.f, 1.f, 1.f));
				triModel.getPoints().setSelectColor(glm::vec3(0.88f, 0.06f, 0.01f));

				triModel.getMesh().setHoverColor(glm::vec3(1.f, 1.f, 1.f));
				triModel.getMesh().setSelectColor(glm::vec3(1.f, 0.06f, 0.51f));

				triModel.getMesh().setColor(glm::vec3(1.f, 1.f, 1.f));
				triModel.getMesh().setMeshSize(0.5f);
			}


		}
		
		// Beuurk !
		if (!once) {
			once = true;
		}

		// std::vector<glm::vec2> points2D = getPoint2D(app.getCamera(), triModel);

		float pointSize = triModel.getPoints().getPointSize();
		ImGui::Text("Point size (in pixel): %d", pointSize);

		// Compute PVM matrix
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, model.getPosition());
		auto pvm = app.getCamera().getProjectionMatrix() * app.getCamera().getViewMatrix() * modelMat;
		glm::vec2 viewport = { app.getScreenWidth(), app.getScreenHeight() };
		
		lineRenderer.clearLines();

		// Compute radius of points in 3D from point size
		std::vector<float> radiuses(m.nverts());
		for (auto &v : m.iter_vertices()) {
			radiuses[v] = getRadius(pvm, sl::um2glm(v.pos()), pointSize, viewport);

			// Push lines for debug
			lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(radiuses[v]), {1.f, 0.f, 0.f}});
		}

		lineRenderer.addLine({{13.37f, -26.5f, 6.5f}, {100.f, 100.f, 100.f}, {1.f, 1.f, 0.5f}});
		lineRenderer.addLine({{0.f, 0.f, 0.f}, {100.f, 100.f, 100.f}, {1.f, 1.f, 0.5f}});
		lineRenderer.push();
		


		// Highlight degenerated points
		PointAttribute<float> pointHl;
		pointHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
		pointHl.fill(0.f);

		// Highlight degenerated facets
		FacetAttribute<float> facetHl;
		facetHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
		facetHl.fill(0.f);

		for (auto &f : m.iter_facets()) {

			// auto isOverlap = getOverlap(app.getCamera(), modelMat, epsScreen, f, points2D);
			
			for (int lv = 0; lv < 3; ++lv) {
				auto a = f.vertex(lv);
				auto b = f.vertex((lv + 1) % 3);

				auto pA = sl::um2glm(a.pos());
				auto pB = sl::um2glm(b.pos());

				if (glm::distance(pA, pB) < radiuses[a] + radiuses[b]) {
					// facetHl[f] = 1.f;
					pointHl[a] = 1.f;
					pointHl[b] = 1.f;
				}
			}

			// // List degenerate facets in the UI
			// if (isFacetDegenerated) {

			// 	// // Goto facet
			// 	// if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_geo_degenerated_facet" + std::to_string(f)).c_str())) {
			// 	// 	Triangle3 t = f;
			// 	// 	cameraGoto(sl::um2glm(t.bary_verts()));
			// 	// }
			// 	// ImGui::SameLine();
			// 	// ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is geo degenerated.");
			// 	// ImGui::SameLine();
			// 	// // ImGui::Text("Dist: %.15g", dist);

			// 	// // Draw which corners are degenerated in the UI
			// 	// drawUIDegeneratedCorners(f, isOverlap);

			// 	facetHl[f] = 1.f;
			// }


			// // Highlight overlap points
			// for (int i = 0; i < 3; ++i) {
			// 	if (isOverlap[i])
			// 		pointHl[f.vertex(i)] = 1;
			// }




		}


		// Update one time by ten
		// static int time = 0;
		// ++time;
		// if (time == 2) {
			// triModel.setHighlight(ElementKind::FACETS);
			triModel.setHighlight(ElementKind::POINTS);
			// time = 0;
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
			glm::mat4 modelMat = glm::mat4(1.0f);
			modelMat = glm::translate(modelMat, model.getPosition());

			Surface::Facet facet(triModel.getTriangles(), tooltipOpen);
			// auto overlaps = getOverlap(app.getCamera(), modelMat, pointSize, facet, points2D);

			ImGui::SetNextWindowPos(tooltipPos);
			ImGui::Begin("Info");

			ImGui::Text("Degenerated facet %i", tooltipOpen);
			ImGui::Text("Overlap vertices:");
			for (int i = 0; i < 3; ++i) {
				// if (overlaps[i]) {
				// 	ImGui::Text("%d", facet.vertex(i));
				// }
			}
			if (ImGui::Button("Close")) {
				tooltipOpen = -1;
			}
			ImGui::End();


		}

		return true;
	}
	


	void update(float dt) {

		// Check that nav path is "diagnostic"
		if (app.getNavigationPath().size() == 0 || app.getNavigationPath().front() != "diagnostic") {
			return;
		}

		glm::vec3 p{0};
		lineRenderer.render(p);
	}

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

	LineRenderer lineRenderer;

};