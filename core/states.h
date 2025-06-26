#pragma once

struct InputState {

	int last_hovered_vertex;
	int last_hovered_edge;
	int last_hovered_facet;
	int last_hovered_cell;

	int hovered_vertex;
	int hovered_edge;
	int hovered_facet;
	int hovered_cell;

	void setHoveredVertex(int vertex) {
		last_hovered_vertex = hovered_vertex;
		hovered_vertex = vertex;
	}

	bool is_vertex_hovered() {
		return hovered_vertex >= 0;
	}

	bool is_vertex_changed() {
		return last_hovered_vertex != hovered_vertex;
	}

	void setHoveredEdge(int edge) {
		last_hovered_edge = hovered_edge;
		hovered_edge = edge;
	}

	bool is_edge_hovered() {
		return hovered_edge >= 0;
	}

	bool is_edge_changed() {
		return last_hovered_edge != hovered_edge;
	}

	void setHoveredFacet(int facet) {
		last_hovered_facet = hovered_facet;
		hovered_facet = facet;
	}

	bool is_facet_hovered() {
		return hovered_facet >= 0;
	}

	bool is_facet_changed() {
		return last_hovered_facet != hovered_facet;
	}

	void setHoveredCell(int cell) {
		last_hovered_cell = hovered_cell;
		hovered_cell = cell;
	}

	bool is_cell_hovered() {
		return hovered_cell >= 0;
	}

	bool is_cell_changed() {
		return last_hovered_cell != hovered_cell;
	}

};