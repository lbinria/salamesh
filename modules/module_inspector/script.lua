function draw_gui() 
	imgui.Begin("Inspector")

	local n_hovered_cells = #app.input_state.cell.all_hovered
	local str_hovered_cell = "X"
	local str_hovered_cells = "X"
	if app.input_state.cell.any_hovered then 
		str_hovered_cell = tostring(app.input_state.cell.hovered)
		for i = 1, n_hovered_cells do 
			if i == 1 then
				str_hovered_cells = tostring(app.input_state.cell.all_hovered[i])
			else
				str_hovered_cells = str_hovered_cells .. ", " .. tostring(app.input_state.cell.all_hovered[i])
			end
		end
	end

	local n_hovered_facets = #app.input_state.facet.all_hovered
	local str_hovered_facet = "X"
	local str_hovered_facets = "X"
	if app.input_state.facet.any_hovered then 
		str_hovered_facet = tostring(app.input_state.facet.hovered)
		for i = 1, n_hovered_facets do 
			if i == 1 then
				str_hovered_facets = tostring(app.input_state.facet.all_hovered[i])
			else
				str_hovered_facets = str_hovered_facets .. ", " .. tostring(app.input_state.facet.all_hovered[i])
			end
		end
	end

	local n_hovered_vertices = #app.input_state.vertex.all_hovered
	local str_hovered_vertex = "X"
	local str_hovered_vertices = "X"
	if app.input_state.vertex.any_hovered then 
		str_hovered_vertex = tostring(app.input_state.vertex.hovered)
		for i = 1, n_hovered_vertices do 
			if i == 1 then
				str_hovered_vertices = tostring(app.input_state.vertex.all_hovered[i])
			else
				str_hovered_vertices = str_hovered_vertices .. ", " .. tostring(app.input_state.vertex.all_hovered[i])
			end
		end
	end

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