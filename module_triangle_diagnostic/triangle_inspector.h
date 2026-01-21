#pragma once 
#include "../core/script.h"
#include "../core/helpers.h"
#include "../core/renderers/line_renderer.h"

#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <optional>

struct TriangleInspector : public Script {

	TriangleInspector(IApp &app) : Script(app) {}	

	// Lifecycle
	void init() {
		lineRenderer.init();
	}

	// virtual void setup() = 0;
	void cleanup() {
		lineRenderer.clean();
	}

	void setupModel() {

		isModelSetup = false;

		// Get current model and check it's a triangle mesh
		auto &model = app.getCurrentModel();
		if (model.getModelType() != ModelType::TRI_MODEL) {
			return;
		}

		auto &triModel = model.as<TriModel>();
		auto &m = triModel.getMesh();

		// Setup gfx
		triModel.unsetColormaps();

		auto &pointRenderer = triModel.getPointsRenderer();
		pointRenderer.setVisible(true);
		// pointRenderer.setColorMode(ColorMode::COLOR);
		pointRenderer.setHoverColor(glm::vec3(1.f, 1.f, 1.f));
		pointRenderer.setSelectColor(glm::vec3(0.88f, 0.06f, 0.01f));

		auto &meshRenderer = triModel.getMeshRenderer();
		meshRenderer.setVisible(true);
		// Highlights
		meshRenderer.setHoverColor(glm::vec3(1.f, 1.f, 1.f));
		meshRenderer.setSelectColor(glm::vec3(1.f, 0.06f, 0.51f));
		// Gfx
		meshRenderer.setVisible(true);
		// meshRenderer.setColorMode(ColorMode::COLOR);
		meshRenderer.setColor(glm::vec3(1.f, 1.f, 1.f));
		meshRenderer.setMeshSize(0.5f);

		auto edgeRenderer = triModel.getEdgesRenderer();
		if (edgeRenderer) {
			edgeRenderer->setVisible(false);
		}


		// Setup bboxes

		auto radiuses = getRadiuses(triModel);

		std::vector<BBox3> bboxes;
		for (auto &v : m.iter_vertices()) {
			BBox3 bb;
			// bb.add(v.pos());
			bb.add(v.pos() - vec3{radiuses[v], radiuses[v], radiuses[v]});
			bb.add(v.pos() + vec3{radiuses[v], radiuses[v], radiuses[v]});
			bboxes.push_back(bb);
		}

		hbbox.init(bboxes);

		isModelSetup = true;
	}

	void modelLoaded(const std::string &path) override {}

	void selectedModelChanged(int idx) override {
		// Save gfx state for restoring ?
		if (isNavHere())
			setupModel();
	}

	

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
		return glm::distance(glm::vec3(worldPosOff), worldPos);
	}

	// Check if nav path is "overlapping_vertices"
	bool isNavHere() {
		return app.getNavigationPathString() == "diagnostic/overlapping_vertices";
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



		bool isNav = isNavHere();

		if (isNav) {
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("Overlapping vertices view")) {
			
			if (app.getNavigationPath().front() == "diagnostic" && app.getNavigationPath().back() != "overlapping_vertices") {
				app.addNavigationPath("overlapping_vertices");

				setupModel();

			}


		}

		if (isNav) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		if (!isNavHere()) {
			ImGui::End();
			return true;
		}
		
		// float pointSize = triModel.getPointsRenderer().getPointSize();

		// // Compute PVM matrix
		// glm::mat4 modelMat = glm::mat4(1.0f);
		// modelMat = glm::translate(modelMat, model.getPosition());
		// auto pvm = app.getCamera().getProjectionMatrix() * app.getCamera().getViewMatrix() * modelMat;
		// glm::vec2 viewport = { app.getScreenWidth(), app.getScreenHeight() };
		
		// lineRenderer.clearLines();

		// // Compute radius of points in 3D from point size
		// std::vector<float> radiuses(m.nverts());
		// for (auto &v : m.iter_vertices()) {
		// 	radiuses[v] = getRadius(pvm, sl::um2glm(v.pos()), pointSize, viewport);

		// 	// Push lines for debug
		// 	lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(radiuses[v], 0.f, 0.f), {1.f, 0.f, 0.f}});
		// 	lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(-radiuses[v], 0.f, 0.f), {1.f, 0.f, 0.f}});
		// 	lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(0.f, radiuses[v], 0.f), {1.f, 0.f, 0.f}});
		// 	lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(0.f, -radiuses[v], 0.f), {1.f, 0.f, 0.f}});
		// 	lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(0.f, 0.f, radiuses[v]), {1.f, 0.f, 0.f}});
		// 	lineRenderer.addLine({sl::um2glm(v.pos()), sl::um2glm(v.pos()) + glm::vec3(0.f, 0.f, -radiuses[v]), {1.f, 0.f, 0.f}});
		// }

		// lineRenderer.push();


		// // Highlight degenerated points
		// PointAttribute<float> pointHl;
		// pointHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getMesh());
		// pointHl.fill(0.f);

		// // Highlight degenerated facets
		// FacetAttribute<float> facetHl;
		// facetHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getMesh());
		// facetHl.fill(0.f);

		// std::vector<std::set<long>> pointOverlaps(m.nverts());
		// int nOverlaps = 0;



		// for (auto &a : m.iter_vertices()) {
			
		// 	BBox3 bbox;
		// 	bbox.add(a.pos() - vec3{radiuses[a], radiuses[a], radiuses[a]});
		// 	bbox.add(a.pos() + vec3{radiuses[a], radiuses[a], radiuses[a]});
		// 	std::vector<int> results;
		// 	hbbox.intersect(bbox, results);

		// 	if (results.size() <= 1)
		// 		continue;

		// 	bool isOverlaps = false;
		// 	for (auto &b : results) {
		// 		if (b == a) continue;

		// 		pointHl[a] = 1.f;
		// 		pointHl[b] = 1.f;
		// 		pointOverlaps[a].insert(b);
		// 		pointOverlaps[b].insert(a);
		// 		isOverlaps = true;
		// 	}
			
		// 	if (isOverlaps)
		// 		++nOverlaps;

		// }


		// triModel.setHighlight(ElementKind::POINTS_ELT);


		if (isNav) {

			// Get current model and check it's a triangle mesh
			auto &model = app.getCurrentModel();
			if (model.getModelType() != ModelType::TRI_MODEL) {
				ImGui::End();
				return true;
			}

			auto &triModel = model.as<TriModel>();
			auto &m = triModel.getMesh();

			float newPointSize = model.getPointsRenderer().getPointSize();
			if (ImGui::SliderFloat("Threshold (point size)", &newPointSize, 0, 50.f)) {
				model.getPointsRenderer().setPointSize(newPointSize);
			}
			ImGui::Text("Overlaps number: %i", nOverlaps);
		}

		ImGui::End();


		auto st = app.getInputState();
		// if (st.mouse.dblButtons[0] && st.vertex.anyHovered()) {
		// 	auto v = st.vertex.getAllHovered().front();
		// 	if (pointHl[v] >= .5f) {
		// 		// Goto
		// 		auto p = sl::um2glm(m.vertex(v).pos());
		// 		auto b = p;
		// 		// Search for bary
		// 		int i = 1;
		// 		for (auto &vOver : pointOverlaps[v]) {
		// 			b += sl::um2glm(m.vertex(vOver).pos());
		// 			++i;
		// 		}
		// 		b /= i;

		// 		// auto eyeP = p - radiuses[v] * 2.f;
		// 		auto eyeP = b - glm::distance(p, b) * 2.f;
		// 		app.getCamera().setEye(eyeP);
		// 		app.getCamera().lookAt(b);
		// 	}
		// }


		// static int tooltipOpen = -1;
		// static ImVec2 tooltipPos;


		// if (st.vertex.anyHovered()) {
		// 	auto v = st.vertex.getAllHovered().front();

		// 	ImGui::BeginTooltip();
		// 	ImGui::Text("Vertex %ld", v);

		// 	if (pointHl[v] >= .5f) {
		// 		ImGui::Text("--- Overlaps ---");
		// 		for (long vi : pointOverlaps[v]) {
		// 			ImGui::Text("vertex %ld", vi);
		// 		}
		// 	}

		// 	ImGui::EndTooltip();
		// }

		// else if (st.facet.anyHovered()) {
		// 	auto f = st.facet.getAllHovered().front();

		// 	ImGui::BeginTooltip();
		// 	ImGui::Text("Facet %ld", f);

		// 	for (int lv = 0; lv < 3; ++lv) {
		// 		int v = m.facet(f).vertex(lv);
		// 		ImGui::Text("- vertex %i", v);
		// 	}

		// 	ImGui::EndTooltip();

		// }


		if (st.edge.anyHovered()) {
			ImGui::BeginTooltip();
			auto e = st.edge.getAllHovered().front();
			ImGui::Text("Edge %ld", e);


			auto &triModel = app.getCurrentModel().as<TriModel>();
			CornerAttribute<double> ca;
			ca.bind("corner_id", triModel.getSurfaceAttributes(), triModel.getMesh());
			
			ImGui::Text("Value %s", std::to_string(ca[e]).c_str());

			ImGui::EndTooltip();
		}

		return true;
	}
	
	std::vector<float> getRadiuses(TriModel &triModel) {
		auto &m = triModel.getMesh();
		float pointSize = triModel.getPointsRenderer().getPointSize();

		// Compute PVM matrix
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, triModel.getPosition());
		auto pvm = app.getCamera().getProjectionMatrix() * app.getCamera().getViewMatrix() * modelMat;
		// TODO important here seems wrong, app.getSurface().width / app.getSurface().height
		glm::vec2 viewport = { app.getScreenWidth(), app.getScreenHeight() };

		std::vector<float> radiuses(m.nverts());
		for (auto &v : m.iter_vertices()) {
			radiuses[v] = getRadius(pvm, sl::um2glm(v.pos()), pointSize, viewport);
		}

		return  radiuses;
	}

	void update(float dt) {

		// Check that nav path is "diagnostic/overlapping_vertices"
		if (!isNavHere() || !app.hasModels() || !isModelSetup)
			return;

		auto &model = app.getCurrentModel();
		if (model.getModelType() != ModelType::TRI_MODEL) {
			return;
		}

		auto &triModel = model.as<TriModel>();
		auto &m = triModel.getMesh();

		float pointSize = triModel.getPointsRenderer().getPointSize();

		// Compute PVM matrix
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, triModel.getPosition());
		auto pvm = app.getCamera().getProjectionMatrix() * app.getCamera().getViewMatrix() * modelMat;
		// TODO important here seems wrong, app.getSurface().width / app.getSurface().height
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
		pointHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getMesh());
		pointHl.fill(0.f);

		// Highlight degenerated facets
		FacetAttribute<float> facetHl;
		facetHl.bind("_highlight", triModel.getSurfaceAttributes(), triModel.getMesh());
		facetHl.fill(0.f);

		std::vector<std::set<long>> pointOverlaps(m.nverts());
		nOverlaps = 0;



		for (auto &a : m.iter_vertices()) {
			
			BBox3 bbox;
			bbox.add(a.pos() - vec3{radiuses[a], radiuses[a], radiuses[a]});
			bbox.add(a.pos() + vec3{radiuses[a], radiuses[a], radiuses[a]});
			std::vector<int> results;
			hbbox.intersect(bbox, results);

			if (results.size() <= 1)
				continue;

			bool isOverlaps = false;
			for (auto &b : results) {
				if (b == a) continue;

				pointHl[a] = 1.f;
				pointHl[b] = 1.f;
				pointOverlaps[a].insert(b);
				pointOverlaps[b].insert(a);
				isOverlaps = true;
			}
			
			if (isOverlaps)
				++nOverlaps;

		}


		triModel.setHighlight(ElementKind::POINTS_ELT);


		glm::vec3 p{0};
		lineRenderer.render(p);

	}

    // Input events
	void mouse_move(double x, double y) {}
	void mouse_button(int button, int action, int mods) {}
    void mouse_scroll(double xoffset, double yoffset) {}
    void key_event(int key, int scancode, int action, int mods) {}

    // App events

	private:

	bool isModelSetup = false;
	double eps = 0.001;
	int nOverlaps = 0;

	LineRenderer lineRenderer;
	HBoxes3 hbbox;

};