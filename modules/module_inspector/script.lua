function init()
	-- print("INITUS")
	-- local idx = app:add_model("my_model")
	-- print("Model added: " .. idx)
	-- app.models[idx]:load("assets/catorus_hex_facet_attr.geogram")
	-- print("Model loaded: " .. idx)
	-- app.models[idx].position = vec3.new(0, 2, 0)


	-- local my_model = app:get_model_by_name("my_model")
	-- print("Name of model my_model: " .. my_model.name)
	-- local my_model_idx = app:get_index_of_model("my_model")
	-- print("Index of my_model: " .. tostring(my_model_idx))

	-- local hello = app:get_model_by_name("hello")

	-- local hello_idx = app:get_index_of_model("hellow")
	-- print("Index of hellow: " .. tostring(hello_idx))
	-- local n = "null"
	-- if hello then n = hello.name end
	-- print("Name of model hello: " .. n)



	-- print("Get by name: " .. my_model .. " at index: " .. my_model_idx)
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
end