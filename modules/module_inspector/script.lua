function get_str(primitive_state)
	local n_hovered = #primitive_state.all_hovered
	local str_hovered_single = "X"
	local str_hovered_many = "X"
	if primitive_state.any_hovered then 
		str_hovered_single = tostring(primitive_state.hovered)
		for i = 1, n_hovered do 
			if i == 1 then
				str_hovered_many = tostring(primitive_state.all_hovered[i])
			else
				str_hovered_many = str_hovered_many .. ", " .. tostring(primitive_state.all_hovered[i])
			end
		end
	end
	return n_hovered, str_hovered_single, str_hovered_many
end

function draw_gui() 
	imgui.Begin("Inspector")

	local n_hovered_cells, str_hovered_cell, str_hovered_cells = get_str(app.input_state.cell)
	local n_hovered_facets, str_hovered_facet, str_hovered_facets = get_str(app.input_state.facet)
	local n_hovered_vertices, str_hovered_vertex, str_hovered_vertices = get_str(app.input_state.vertex)

	local str_hovered_edge = "X"
	if app.input_state.edge.any_hovered then 
		str_hovered_edge = tostring(app.input_state.edge.hovered)
	end

	imgui.Text("Hovered cell: " .. str_hovered_cell)
	imgui.Text("Hovered facet: " .. str_hovered_facet)
	imgui.Text("Hovered edge: " .. str_hovered_edge)
	imgui.Text("Hovered vertex: " .. str_hovered_vertex)

	imgui.Separator()
	imgui.Text(tostring(n_hovered_cells) .. " hovered cells: " .. "{" .. str_hovered_cells .. "}")
	imgui.Text(tostring(n_hovered_facets) .. " hovered facets: " .. "{" .. str_hovered_facets .. "}")
	imgui.Text(tostring(n_hovered_vertices) .. " hovered vertices: " .. "{" .. str_hovered_vertices .. "}")


	imgui.End()
end