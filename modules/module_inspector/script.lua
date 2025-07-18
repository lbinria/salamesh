function draw_gui() 
	imgui.Begin("Inspector")

	local str_hovered_cell = "X"
	if app.input_state.cells.is_hovered then 
		str_hovered_cell = tostring(app.input_state.cells.hovered)
	end

	local str_hovered_facet = "X"
	if app.input_state.facets.is_hovered then 
		str_hovered_facet = tostring(app.input_state.facets.hovered)
	end

	local str_hovered_edge = "X"
	if app.input_state.edge.is_hovered then 
		str_hovered_edge = tostring(app.input_state.edge.hovered)
	end

	imgui.Text("Hovered cell: " .. str_hovered_cell)
	imgui.Text("Hovered facet: " .. str_hovered_facet)
	imgui.Text("Hovered edge: " .. str_hovered_edge)

	imgui.End()
end