
-- local pick_state = nil
function mouse_move(x, y)
	
	-- local mouse_state = app.input_state.mouse 
	-- pick_state = app.scene.default_render_surface:get_pick_state(mouse_state.pos.x, mouse_state.pos.y, mouse_state.cursor_radius)

end

-- Format a list of ids as a string
function format_str_ids(ids) 
	local str = ""
	for i = 1, #ids do 
		if i == 1 then
			str = tostring(ids[i])
		else
			str = str .. ", " .. tostring(ids[i])
		end
	end
	return str
end

function draw_gui() 

	imgui.Begin("Inspector II")

		local pick_state = app.input_state.pick

		-- Get picked meshes
		local mesh_ids = pick_state:get_ids(PickElement.PICK_MESH)

		-- Loop through picked meshes
		for i, mesh_id in ipairs(mesh_ids) do

			-- local mesh = app.scene:find_geometry_by_index(mesh_id)
			-- local model = app.scene:find_node_by_mesh_index(mesh_id)
			-- model.name

			imgui.Text("Mesh " .. tostring(mesh_id))

			for element = 0, PickElement.PICK_ELEMENT_COUNT - 2 do 
				local ids = pick_state:get_ids(mesh_id, element)
				imgui.Text(tostring(#ids) .. " hovered " .. pick_element_to_string(element) .. ": {" .. format_str_ids(ids) .. "}")
				
			end

			imgui.Separator()
		end




	imgui.End()
end