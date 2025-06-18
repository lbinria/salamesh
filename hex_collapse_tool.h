#pragma once

#include <ultimaille/all.h>
#include "core/component.h"

#include "boundaries.h"
#include "model.h"
#include "states.h"
#include "hex_renderer.h"

struct HexCollapseTool : public Component {

	HexCollapseTool(Hexahedra &hex, HexRenderer &hex_renderer, InputState &st) :
		hex(hex), 
		// cell_highlights_attr(hex, 0.f),
		hex_renderer(hex_renderer), 
		st(st) {}

	void init() final override;
	void setup() final override;
	void cleanup() final override;

	bool draw_gui() final override;

	void hover_callback(double x, double y, int source);

	// void edge_hover(std::vector<int> ids) override;

	void mouse_move(double x, double y) final override;
	void mouse_button(int button, int action, int mods) final override;
	void mouse_scroll(double xoffset, double yoffset) final override {}
	void key_event(int key, int scancode, int action, int mods) final override;
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