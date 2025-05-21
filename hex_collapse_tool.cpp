#include "hex_collapse_tool.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

void HexCollapseTool::init() {
	std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
	hex.connect();

	// cell_highlights_attr.ptr->resize(hex.ncells());
	cell_highlights_attr.resize(hex.ncells(), 0.f);
	cell_highlights_attr.assign(hex.ncells(), 0.f);
	hex_renderer.setHighlight(cell_highlights_attr);

	std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
	std::cout << "init in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - begin2).count() << "ms" << std::endl;

	is_init = true;
}

void HexCollapseTool::setup() {

	std::chrono::steady_clock::time_point bt = std::chrono::steady_clock::now();

	// Compute hex layers
	DisjointSet ds(hex.ncells() * 24);

	for (auto h : hex.iter_halfedges()) {
		auto opp = h.opposite_f().opposite_c();
		if (opp.active())
			ds.merge(h, opp);
		
		ds.merge(h, h.next().next().opposite_f());
	}

	ds.get_sets_id(layers);

	std::chrono::steady_clock::time_point et = std::chrono::steady_clock::now();
	
	std::cout << 
		"setup in: " << 
		std::chrono::duration_cast<std::chrono::milliseconds>(et - bt).count() << 
		"ms" << 
	std::endl;

	is_setup = true;
}

void HexCollapseTool::cleanup() {

}

void HexCollapseTool::draw_gui() {

	if (ImGui::Button("Select hex layer to collapse")) {
		init();
		setup();
	}

	ImGui::Checkbox("Auto smooth##chk_auto_smooth_collapse_tool", &auto_smooth);
}

void HexCollapseTool::mouse_move(double x, double y) {
	if (!st.is_edge_hovered() || !st.is_edge_changed()) 
		return;

	// CellFacetAttribute<float> cell_highlights_attr(hex, 0.f);

	// Remove last hovered cells
	for (auto c : last_hovered_cells) {
		if (cell_highlights_attr[c] >= 1.f && cell_highlights_attr[c] < 2.f)
			cell_highlights_attr[c] = 0.f;
	}

	hovered_cells.clear();
	for (auto h : hex.iter_halfedges()) {
		if (layers[h] == layers[st.hovered_edge])
			hovered_cells.push_back(h.cell());
	}

	last_hovered_cells = hovered_cells;

	// Set new hovered cells
	for (auto c : hovered_cells) {
		if (cell_highlights_attr[c] <= 0.f)
			cell_highlights_attr[c] = 1.f;
	}

	hex_renderer.setHighlight(cell_highlights_attr);
	// hex_renderer.setHighlight(cell_highlights_attr.ptr->data);
}

void HexCollapseTool::mouse_button(int button, int action, int mods) {

	if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS || !st.is_edge_hovered())
		return;

	// CellFacetAttribute<float> cell_highlights_attr(hex, 0.f);

	// Remove last selected
	for (auto c : selected_cells) {
		if (cell_highlights_attr[c] >= 2.f)
			cell_highlights_attr[c] = 0.f;
	}

	selected_cells = hovered_cells;


	for (auto c : selected_cells) {
		cell_highlights_attr[c] = 2.f;
	}

	// hex_renderer.setHighlight(cell_highlights_attr.ptr->data);
	hex_renderer.setHighlight(cell_highlights_attr);

	selected_layer = layers[st.hovered_edge];
}

void HexCollapseTool::key_event(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		validate_callback();
	}
}

void HexCollapseTool::validate_callback() {
	
	collapse();

	// if (auto_smooth)
	// 	BenjaminAPI::smooth(ctx.hex_bound->hex, *ctx.emb_attr, ctx.tet_bound->tri, *ctx.tri_chart);

	// Update rendering
	// Time elapsed
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	hex_renderer.to_gl(hex);
	hex_renderer.update();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "update in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;

	// Reinit
	init();
	setup();

	

	hovered_cells.clear();
	last_hovered_cells.clear();
	
}

void HexCollapseTool::collapse(/*, CellFacetAttribute<int> &emb_attr*/) {
	
	std::vector<int> layer;

	for (auto h : hex.iter_halfedges()) {
		if (layers[h] != selected_layer)
			continue;

		layer.push_back(h);

		auto nh = h.opposite_f().next().next();
		layer.push_back(nh);
		nh = nh.opposite_f().next().next();
		layer.push_back(nh);
		nh = nh.opposite_f().next().next();
		layer.push_back(nh);

	}


	std::vector<int> indirection_map(hex.nverts());
	std::vector<bool> cells_to_kill(hex.ncells(), false);
	{
		for (auto v : hex.iter_vertices())
			indirection_map[v] = v;


		bool is_forward_collapse = true;
		bool is_backward_collapse = true;
		for (auto h_idx : layer) {
			Volume::Halfedge h(hex, h_idx);
			
			for (auto ha : h.iter_CCW_around_edge()) {
				if (!ha.next().opposite_f().opposite_c().active()) {
					is_forward_collapse = false;
					break;
				}
				if (!ha.prev().opposite_f().opposite_c().active()) {
					is_backward_collapse = false;
					break;
				}
			}


			if (!is_forward_collapse && !is_backward_collapse) {
				// "Hard" to collapse
				return;
			}
		}

		for (auto h_idx : layer) {
			Volume::Halfedge h(hex, h_idx);
			if (is_backward_collapse)
				indirection_map[h.from()] = h.to();
			else 
				indirection_map[h.to()] = h.from();

			cells_to_kill[h.cell()] = true;
		}
	}


	for (int c = 0; c < hex.ncells(); c++) {
		for (int lv = 0; lv < 8; lv++) {
			hex.vert(c, lv) = indirection_map[hex.vert(c, lv)];
		}
	}

	// // propagate embedding
	// {
	// 	bool done = false;
	// 	while (!done) {
	// 		done = true;
	// 		for (auto f : hex.iter_facets()) {
	// 			// If cell is to kill and facet has an embedding (>= 0)
	// 			if (emb_attr[f] < 0 || !cells_to_kill[f.cell()]) 
	// 				continue;
				
	// 			// We move its embedding to the opposite interior facet
	// 			auto f_next = f.halfedge(0).opposite_f().next().next().opposite_f().facet().opposite();
				
	// 			if (f_next.active()) {
	// 				std::swap(emb_attr[f], emb_attr[f_next]);
	// 				done = false;
	// 			}
	// 		}
	// 	}
	// }



	// Remove degenerate hex
	hex.disconnect();
	hex.delete_cells(cells_to_kill);

	// Remove isolated vertices
	hex.delete_isolated_vertices();


}