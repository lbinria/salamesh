function init()
	print("Screen width: " .. tostring(app.width))
	print("Screen height: " .. tostring(app.height))
	print("Screen aspect: " .. tostring(app.aspect_ratio))
	print("Number of cameras: " .. tostring(app.count_cameras))


	-- local j = app.camera:save_state()
	-- app.camera:savus(j)
end

local val = 0
function draw_gui()
	-- imgui.Begin("Foot_bar")
	-- imgui.Text(app.navigation_path_string)
	-- imgui.End()

	-- if app.has_models then

	-- 	if imgui.Button("Filter !!!") then 
	-- 		filter()
	-- 	end

	-- end

	imgui.Begin("yop !")
	 
	if imgui.Button("plop") then
		filter()
	end

	if imgui.Button("top") then 
		app:top_navigation_path()
	end

	if imgui.Button("Remove attr") then 
		-- local attr0 = app.model:get_attr(0)
		-- print(attr0.name)

		-- local attr1 = app.model:get_attr(ElementKind.POINTS_ELT, "point_id")
		-- print(attr1.name)

		-- app.model:remove_attr(0)
		-- app.model:remove_attr(ElementKind.FACETS_ELT, "facet_id")
		app.model:clear_attrs()
	end

	imgui.End()

end

function filter()
	local tri_model = app.model:as_tri()
	local fa = FacetAttributeDouble.new()
	
	fa:bind("_filter", tri_model.surface_attributes, tri_model.mesh)

	for i=0,1000 do 
		fa[i] = 1.0
	end
	
	-- app.model:set_filter(ElementKind.FACETS_ELT, true)
	tri_model:set_filter(ElementKind.FACETS_ELT, true)
end


function update(dt)
	-- local r = app.camera.right_vector
	-- local u = app.camera.up_vector
	-- local f = app.camera.view_dir
	-- print("Right vector: (" .. r[1] .. "," .. r[2] .. "," .. r[3] .. ")")
	-- print("Up vector: (" .. u[1] .. "," .. u[2] .. "," .. u[3] .. ")")
	-- print("Forward vector: (" .. f[1] .. "," .. f[2] .. "," .. f[3] .. ")")
	-- local proj = app.camera.view_matrix
	-- print("Projection: ")
	-- local s = "("
	-- for i=1,4 do 
	-- 	for j=1,4 do 
	-- 		s = s .. proj[i][j] .. ","
	-- 	end
	-- 	s = s .."\n"
	-- end 
	-- s = s .. ")"
	-- print(s)

	
	-- app.camera:move({0,0}, {1600.0 * dt, 0})


end