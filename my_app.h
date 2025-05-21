#include "app.h"
#include "debug_tool.h"
#include "hex_collapse_tool.h"
#include "lua_engine.h"
#include "script.h"
#include "lua_console.h"

struct MyApp : App {

	MyApp() : App(), lua_engine(*this, st), lua_script(*this, "../module_a_test/script.lua") {}

	int pick_edge(Volume &m, glm::vec3 p0, int c) {
		// Search nearest edge
		double min_d = std::numeric_limits<double>().max();
		int found_e = -1;
		
		for (int le = 0; le < 24; ++le) {
			int e = c * 24 + le;

			// Get local points indices of edge extremities
			int lv0 = reference_cells[1].facets[le % 4 + (le / 4 * 4)];
			int lv1 = reference_cells[1].facets[(le + 1) % 4 + (le / 4 * 4)];

			// std::cout << "le: " << le << ", lv0: " << lv0 << ", lv1: " << lv1 << std::endl;

			// Get global index of points
			int v0 = m.vert(c, lv0);
			int v1 = m.vert(c, lv1);

			// std::cout << "v0: " << v0 << ", v1: " << v1 << std::endl;

			// Get points from current edge
			vec3 p1 = m.points[v0];
			vec3 p2 = m.points[v1];
			vec3 b = (p1 + p2) * .5;
			// mesh.setPoint(glm::vec3(b.x, b.y, b.z));
			// Compute dist from picked point to bary of edge points
			// double d = (vec3(p0.x, p0.y, p0.z) - ((p1 + p2) / 2.)).norm2();
			double d = (vec3(p0.x, p0.y, p0.z) - b).norm();

			// std::cout << "p0: " << p0.x << ", " << p0.y << ", " << p0.z << std::endl;
			// std::cout << "b: " << b.x << ", " << b.y << ", " << b.z << std::endl;
			// std::cout << "dist: " << d << std::endl;

			// Keep min dist
			if (d < min_d) {
				min_d = d;
				found_e = e;
			}
		}

		return found_e;
	}

	// Override lifecycle functions
	void init() override;
	void update(float dt) override;
	void draw_gui() override;

	// Override event functions
    void key_event(int key, int scancode, int action, int mods) override;
	void mouse_scroll(double xoffset, double yoffset) override;
	void mouse_button(int button, int action, int mods) override;
	void mouse_move(double x, double y) override;
	void mouse_drag(int button, double x, double y) override;

	private:

	// std::unique_ptr<Model> model;
	InputState st;

	// std::unique_ptr<HexCollapseTool> hex_collapse_tool;

	std::unique_ptr<LuaConsole> lua_console;

	// TODO move to app and abstract std::vector<Engine>, std::vector<Script>
	LuaEngine lua_engine;
	LuaScript lua_script;

};