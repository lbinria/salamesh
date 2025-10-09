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

	// // Get 2D screen pos of all points of the model
	// std::vector<glm::vec2> getPoint2D(Camera &c, TriModel &model) const {
	// 	std::vector<glm::vec2> points2D;
	// 	points2D.reserve(model.nverts());
		
	// 	for (int i = 0; i < model.nverts(); ++i) {
	// 		vec3 p3 = model.getTriangles().points[i];
	// 		glm::vec4 p(p3.x, p3.y, p3.z, 1.f);
			
	// 		p = c.getProjectionMatrix() * c.getViewMatrix() * p;
	// 		p /= p.w;
			
	// 		p.x = (p.x + 1.f) * 0.5f * app.getScreenWidth();
	// 		p.y = (1.f - (p.y + 1.f) * 0.5f) * app.getScreenHeight();

	// 		points2D.push_back(glm::vec2(p.x, p.y));
	// 	}
	// 	return points2D;	
	// }

	

	// Lifecycle
	void init() {
		lineRenderer.init();
	}

	// virtual void setup() = 0;
	void cleanup() {
		lineRenderer.clean();
	}

	void modelLoaded(const std::string &path) override {}


	// Get the radius of a point in world-space cartesian coordinates
	// Given its point size in pixel screen-space coordinates
	float getRadius(glm::mat4 &pvm, glm::vec3 worldPos, float pointSize, glm::vec2 &viewport) {
		
		// ---- 1. Project point to screen ----
		// Clip space
		glm::vec4 clip = pvm * glm::vec4(worldPos, 1.0);
		// Compute screen pos (2D) in pixel of center of point
		glm::vec4 ndc = clip / clip.w;
		glm::vec2 screen = (glm::vec2{ndc.x, ndc.y} * 0.5f + 0.5f) * viewport;

		// ---- 2. Move point by half of point size ----
		screen.x += pointSize * .5f;

		// ---- 3. Unproject offset point to world space ----
		// Back to NDC
		glm::vec2 ndcOff = screen / viewport * 2.f - 1.f;
		// Inverse of perspective division, return to clip space
		glm::vec4 clipOff = glm::vec4(ndcOff * clip.w, clip.z, clip.w);
		// Back to world space (homogeneous coordinates)
		glm::mat4 invPVM = glm::inverse(pvm);
		glm::vec4 worldPosOff = invPVM * clipOff;
		// Homogeneous coordinates to Cartesian coordinates
		worldPosOff /= worldPosOff.w;

		// Above can be replaced by
		// worldPosOff = glm::unProject(screen, vm, p, viewport);

		// ---- 4. Compute distance between point and offset point in world-space cartesian coordinates ----
		return glm::length(glm::vec3(worldPosOff) - glm::vec3(worldPos));
	}


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

		if (ImGui::Button("Overlapping vertices view")) {
			
			if (app.getNavigationPath().front() == "diagnostic" && app.getNavigationPath().back() != "overlapping_vertices") {
				app.addNavigationPath("overlapping_vertices");

				// Setup gfx
				triModel.getPoints().setVisible(true);
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


		float pointSize = triModel.getPoints().getPointSize();

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
			lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(radiuses[v], 0.f, 0.f), {1.f, 0.f, 0.f}});
			lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(-radiuses[v], 0.f, 0.f), {1.f, 0.f, 0.f}});
			lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(0.f, radiuses[v], 0.f), {1.f, 0.f, 0.f}});
			lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(0.f, -radiuses[v], 0.f), {1.f, 0.f, 0.f}});
			lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(0.f, 0.f, radiuses[v]), {1.f, 0.f, 0.f}});
			lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(0.f, 0.f, -radiuses[v]), {1.f, 0.f, 0.f}});
		}

		lineRenderer.push();
		


		// Highlight degenerated points
		PointAttribute<float> pointHl;
		pointHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
		pointHl.fill(0.f);

		// Highlight degenerated facets
		FacetAttribute<float> facetHl;
		facetHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getTriangles());
		facetHl.fill(0.f);

		std::vector<std::vector<long>> pointOverlaps(m.nverts());

		int nOverlaps = 0;
		for (auto &f : m.iter_facets()) {
			
			for (int lv = 0; lv < 3; ++lv) {
				auto a = f.vertex(lv);
				auto b = f.vertex((lv + 1) % 3);

				auto pA = sl::um2glm(a.pos());
				auto pB = sl::um2glm(b.pos());

				if (glm::distance(pA, pB) < radiuses[a] + radiuses[b]) {
					// facetHl[f] = 1.f;
					pointHl[a] = 1.f;
					pointHl[b] = 1.f;

					pointOverlaps[a].push_back(b);
					pointOverlaps[b].push_back(a);
					++nOverlaps;
				}
			}

			facetHl[f] = .5f;

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


		triModel.setHighlight(ElementKind::POINTS);



		if (app.getNavigationPathString() == "diagnostic/overlapping_vertices") {
			float newPointSize = model.getPoints().getPointSize();
			if (ImGui::SliderFloat("Threshold (point size)", &newPointSize, 0, 50.f)) {
				model.getPoints().setPointSize(newPointSize);
			}
			ImGui::Text("Overlaps number: %i", nOverlaps);

			static int vertexId = -1;
			ImGui::InputInt("Go to vertex: ", &vertexId);
			ImGui::SameLine();
			if (ImGui::SmallButton("go")) {
				auto v = m.vertex(vertexId);
				app.getCamera().lookAt(sl::um2glm(v.pos()));
			}

			if (vertexId >= 0 && vertexId < m.nverts()) {
				auto p =  m.vertex(vertexId).pos();
				ImGui::Text("x: %f, y: %f, z: %f", p.x, p.y, p.z);
			}
		}

		ImGui::End();


		auto st = app.getInputState();
		if (st.mouse.dblButtons[0] && st.vertex.anyHovered()) {
			auto v = st.vertex.getAllHovered().front();
			if (pointHl[v] >= .5f) {
				// Goto
				auto p = sl::um2glm(m.vertex(v).pos());
				auto b = p;
				// Search for bary
				int i = 1;
				for (auto &vOver : pointOverlaps[v]) {
					b += sl::um2glm(m.vertex(vOver).pos());
					++i;
				}
				b /= i;

				// auto eyeP = p - radiuses[v] * 2.f;
				auto eyeP = b - glm::distance(p, b) * 2.f;
				app.getCamera().setEye(eyeP);
				app.getCamera().lookAt(b);
			}
		}


		//
		static int tooltipOpen = -1;
		static ImVec2 tooltipPos;
		// if (app.getInputState().facet.anyHovered() && tooltipOpen == -1) {
		// 	auto f = app.getInputState().facet.getAllHovered().front();
		// 	if (facetHl[f] >= 0.5f && tooltipOpen == -1) {
		// 		ImGui::BeginTooltip();
		// 		ImGui::Text("Degenerated facet %ld", f);
		// 		ImGui::Text("Click to open...");
		// 		ImGui::EndTooltip();

		// 		// if (app.getInputState().mouse.isLeftButton()) {
		// 		// 	tooltipOpen = f;
		// 		// 	tooltipPos = ImGui::GetMousePos();
		// 		// }
		// 	}
		// }

		if (st.vertex.anyHovered()) {
			auto v = st.vertex.getAllHovered().front();

			ImGui::BeginTooltip();
			ImGui::Text("Vertex %ld", v);

			if (pointHl[v] >= .5f) {
				ImGui::Text("--- Overlaps ---");
				for (long vi : pointOverlaps[v]) {
					ImGui::Text("vertex %ld", vi);
				}
			}

			ImGui::EndTooltip();
		}

		if (st.facet.anyHovered()) {
			auto f = st.facet.getAllHovered().front();

			ImGui::BeginTooltip();
			ImGui::Text("Facet %ld", f);

			for (int lv = 0; lv < 3; ++lv) {
				int v = m.facet(f).vertex(lv);
				ImGui::Text("- vertex %i", v);
			}

			ImGui::EndTooltip();

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