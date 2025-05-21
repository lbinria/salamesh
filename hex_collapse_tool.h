#pragma once

#include <ultimaille/all.h>
#include "tool.h"

#include "boundaries.h"
#include "model.h"
#include "states.h"
#include "hex_mesh.h"

struct HexCollapseTool : public Tool {

	HexCollapseTool(Hexahedra &hex, HexRenderer &hex_renderer, InputState &st) : Tool(), 
		hex(hex), 
		// cell_highlights_attr(hex, 0.f),
		hex_renderer(hex_renderer), 
		st(st) {}

	void init() override;
	void setup() override;
	void cleanup() override;

	void draw_gui() override;

	void hover_callback(double x, double y, int source);

	// void edge_hover(std::vector<int> ids) override;

	void mouse_move(double x, double y) override;
	void mouse_button(int button, int action, int mods) override;
	void key_event(int key, int scancode, int action, int mods) override;
	void validate_callback(); // TODO map validate to a key, enable user to configure the key

	private:

	Hexahedra &hex;
	HexRenderer &hex_renderer;
	InputState &st;

	bool is_init;
	bool is_setup;

	// CellFacetAttribute<float> cell_highlights_attr;
	std::vector<float> cell_highlights_attr;

	std::vector<int> layers;
	std::vector<int> last_hovered_cells;
	std::vector<int> hovered_cells;
	std::vector<int> selected_cells;
	int selected_layer = -1;

	bool auto_smooth = false;

	void collapse();

};