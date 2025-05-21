#pragma once

#include <ultimaille/all.h>

using namespace UM;

struct InputState {

	int last_hovered_edge;

	int hovered_cell;
	int hovered_edge;

	bool is_cell_hovered() {
		return hovered_cell >= 0;
	}

	bool is_edge_hovered() {
		return hovered_edge >= 0;
	}

	bool is_edge_changed() {
		return last_hovered_edge != hovered_edge;
	}

};