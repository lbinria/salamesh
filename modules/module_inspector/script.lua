function init()

end

function get_str(primitive_state)
	local n_hovered = #primitive_state.all_hovered
	local str_hovered_many = "X"
	if primitive_state.any_hovered then 
		for i = 1, n_hovered do 
			if i == 1 then
				str_hovered_many = tostring(primitive_state.all_hovered[i])
			else
				str_hovered_many = str_hovered_many .. ", " .. tostring(primitive_state.all_hovered[i])
			end
		end
	end
	return n_hovered, str_hovered_many
end

-- local pick_state = nil
function mouse_move(x, y)
	
	-- local mouse_state = app.input_state.mouse 
	-- pick_state = app.scene.default_render_surface:get_pick_state(mouse_state.pos.x, mouse_state.pos.y, mouse_state.cursor_radius)

end

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
	imgui.Begin("Inspector")

	local n_hovered_cells, str_hovered_cells = get_str(app.input_state.cell)
	local n_hovered_facets, str_hovered_facets = get_str(app.input_state.facet)
	local n_hovered_vertices, str_hovered_vertices = get_str(app.input_state.vertex)
	local _, str_hovered_mesh = get_str(app.input_state.mesh)

	local str_hovered_edge = "X"
	if app.input_state.edge.any_hovered then 
		str_hovered_edge = tostring(app.input_state.edge.hovered)
	end

	if str_hovered_mesh == "-1" then str_hovered_mesh = "X" end

	imgui.Text("Hovered mesh: " .. str_hovered_mesh)
	imgui.Text(tostring(n_hovered_cells) .. " hovered cells: " .. "{" .. str_hovered_cells .. "}")
	imgui.Text(tostring(n_hovered_facets) .. " hovered facets: " .. "{" .. str_hovered_facets .. "}")
	imgui.Text(tostring(n_hovered_vertices) .. " hovered vertices: " .. "{" .. str_hovered_vertices .. "}")
	imgui.Text("Nearest edge: " .. str_hovered_edge)



	imgui.End()

	imgui.Begin("Inspector II")


		local pick_state = app.input_state.pick

		local mesh_ids = pick_state:get_ids(PickElement.PICK_MESH)



		for i, mesh_id in ipairs(mesh_ids) do

			-- local mesh = app.scene:find_geometry_by_index(mesh_id)
			-- local model = app.scene:find_node_by_mesh_index(mesh_id)
			-- model.name

			imgui.Text("Mesh " .. tostring(mesh_id))

			local vertices_ids = pick_state:get_ids(mesh_id, PickElement.PICK_VERTEX)
			local halfedge_ids = pick_state:get_ids(mesh_id, PickElement.PICK_HALFEDGE)
			local facet_ids = pick_state:get_ids(mesh_id, PickElement.PICK_FACET)

			imgui.Text(tostring(#vertices_ids) .. " hovered vertices: {" .. format_str_ids(vertices_ids) .. "}")
			imgui.Text(tostring(#halfedge_ids) .. " hovered halfedge: {" .. format_str_ids(halfedge_ids) .. "}")
			imgui.Text(tostring(#facet_ids) .. " hovered facets: {" .. format_str_ids(facet_ids) .. "}")

			imgui.Separator()
		end




	imgui.End()
end