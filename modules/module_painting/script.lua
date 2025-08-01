function draw_gui() 

	imgui.Begin("Painting Inspector")

	if app.input_state.cell.any_hovered then

		for _, cell in ipairs(app.input_state.cell.all_hovered) do
			if cell then
				-- app.model:get_attr("paint", 3, 0):set(cell, 1)
			end
		end
	end

	imgui.End()

end