#pragma once 
#include "core/component.h"

#include <string>
#include <random>
#include <sstream>
#include <iomanip>
struct TriangleInspector : public Component {

	TriangleInspector(IApp &app) : app(app) {}

	glm::vec3 um2glm(UM::vec3 v) {
		return glm::vec3(v.x, v.y, v.z);
	}

	std::string uuid_v4()
	{
		static std::random_device rd;
		static std::mt19937_64 gen(rd());
		static std::uniform_int_distribution<uint64_t> dist;

		std::array<uint8_t, 16> bytes;
		uint64_t a = dist(gen);
		uint64_t b = dist(gen);
		for (int i = 0; i < 8; ++i) bytes[i] = static_cast<uint8_t>((a >> (i*8)) & 0xFF);
		for (int i = 0; i < 8; ++i) bytes[8 + i] = static_cast<uint8_t>((b >> (i*8)) & 0xFF);

		// Set version = 4 (bits 12-15 of time_hi_and_version)
		bytes[6] = (bytes[6] & 0x0F) | 0x40;
		// Set variant = 10xx (bits 6-7 of clock_seq_hi_and_reserved)
		bytes[8] = (bytes[8] & 0x3F) | 0x80;

		std::ostringstream oss;
		oss << std::hex << std::setfill('0');
		for (size_t i = 0; i < bytes.size(); ++i)
		{
			oss << std::setw(2) << static_cast<int>(bytes[i]);
			if (i == 3 || i == 5 || i == 7 || i == 9) oss << '-';
		}
		return oss.str();
	}
	std::string uuid_to_hex() {
		std::string u = uuid_v4(); // random UUID (libuuid)
		std::ostringstream ss;
		ss << std::hex << std::setfill('0');
		for (int i = 0; i < 16; ++i) ss << std::setw(2) << (static_cast<int>(u[i]) & 0xff);
		return ss.str(); // 32 hex chars, no dashes
	}

	void cameraGoto(glm::vec3 p) {
		auto dir = glm::normalize(app.getCamera().getEye() - p);
		glm::vec3 newPos = p - dir * 1.f; 
		app.getCamera().setEye(newPos);
		app.getCamera().lookAt(p);
	}

	void displayDegeneratedCorners(Surface::Facet &f, bool degenerated[3]) {
		for (int i = 0; i < 3; ++i) {

			if (!degenerated[i]) continue;

			// std::string lnkLabel = "corner " 
			// 	+ std::to_string(i) 
			// 	+ " (vert " 
			// 	+ std::to_string(f.vertex(i)) 
			// 	+ ")##link_goto_degenerated_vertex" 
			// 	+ std::to_string(f.vertex(i))
			// 	+ uuid_v4();
			std::string lnkLabel = "corner " 
				+ std::to_string(i) 
				+ " (vert " 
				+ std::to_string(f.vertex(i)) 
				+ ")##link_goto_vert_"
				+ std::to_string(f.vertex(i));

			ImGui::SameLine();
			if (ImGui::TextLink(lnkLabel.c_str())) {
				cameraGoto(um2glm(f.vertex(i).pos()));
			}
		}
	}

    // Lifecycle
    void init() {}
    // virtual void setup() = 0;
	void cleanup() {}
    
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

		auto &tri_model = model.as<TriModel>();
		auto &m = tri_model.getTriangles();
		
		// Eps for min distance square between two points, to be recognized a triangle as geometrically degenerated
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

				if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_topo_degenerated_facet_" + std::to_string(f)).c_str())) {
					Triangle3 t = f;
					cameraGoto(um2glm(t.bary_verts()));
				}
				ImGui::SameLine();

				ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is topo degenerated. Following corners coincide: ");

				displayDegeneratedCorners(f, topo_degenerated);
			}

			bool geo_degenerated[3] = {false};

			auto p0 = f.vertex(0).pos();
			auto p1 = f.vertex(1).pos();
			auto p2 = f.vertex(2).pos();

			// Check for points
			float p0p1Dist = (p0 - p1).norm2(); 
			float p0p2Dist = (p0 - p2).norm2(); 
			float p1p2Dist = (p1 - p2).norm2();

			if (p0p1Dist < eps) {
				geo_degenerated[0] = true;
				geo_degenerated[1] = true;
			}
			if (p0p2Dist < eps) {
				geo_degenerated[0] = true;
				geo_degenerated[2] = true;
			}
			if (p1p2Dist < eps) {
				geo_degenerated[1] = true;
				geo_degenerated[2] = true;
			}

			bool isGeoDegenerated = geo_degenerated[0] || geo_degenerated[1] || geo_degenerated[2];


			
			if (!isTopoDegenerated && (p0p1Dist < eps || p0p2Dist < eps || p1p2Dist < eps)) {

				PointAttribute<float> pointHl;
				pointHl.bind("_highlight", tri_model.getSurfaceAttributes(), tri_model.getTriangles());

				if (geo_degenerated[0]) {
					pointHl[f.vertex(0)] = 0.1f;
				}

				tri_model.setHighlight(ElementKind::POINTS);

				if (ImGui::TextLink(("Facet " + std::to_string(f) + "##link_goto_geo_degenerated_facet" + std::to_string(f)).c_str())) {
					Triangle3 t = f;
					cameraGoto(um2glm(t.bary_verts()));
				}
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), " is geo degenerated.");

				displayDegeneratedCorners(f, geo_degenerated);

				ImGui::SameLine();
				if (ImGui::TextLink(("Hide other facets##link_show_only_geo_degenerated_facet" + std::to_string(f)).c_str())) {
					FacetAttribute<float> filter;
					filter.bind("_filter", tri_model.getSurfaceAttributes(), m);

					CornerAttribute<float> cornerHl;
					cornerHl.bind("_highlight", tri_model.getSurfaceAttributes(), m);

					for (auto &cf : m.iter_facets()) {
						if (cf != f) {
							filter[cf] = 1.f;


						} else {
							for (int i = 0; i < 3; ++i) {
								int cornerIdx = cf * 3 + i;
								cornerHl[cornerIdx] = 0.1f;
							}
						}
					}

					model.setFilter(ElementKind::FACETS);
					model.setHighlight(ElementKind::CORNERS);

					// Goto facet
					Triangle3 t = f;
					cameraGoto(um2glm(t.bary_verts()));
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