
function init()
	print("Screen width: " .. tostring(app.width))
	print("Screen height: " .. tostring(app.height))
	print("Screen aspect: " .. tostring(app.aspect_ratio))
	print("Number of cameras: " .. tostring(app.count_cameras))

	app:add_renderer("LineRenderer", "line_renderer")
	app:add_renderer("PointSetRenderer", "my_point_set_renderer")
	-- local j = app.camera:save_state()
	-- app.camera:savus(j)

	-- local av_cameras = app:list_available_cameras()
	-- local av_renderers = app:list_available_renderers()
	local av_models = app.model_instanciator:list_available_types()
	local av_cameras = app.camera_instanciator:list_available_types()
	local av_renderers = app.renderer_instanciator:list_available_types()

	print("Available models list:")
	for i=1, #av_models do
		print(av_models[i])
	end

	print("Available cameras list:")
	for i=1, #av_cameras do
		print(av_cameras[i])
	end

	print("Available renderers list:")
	for i=1, #av_renderers do
		print(av_renderers[i])
	end

	for n, r in pairs(app.renderers) do
		print("Renderer: " .. n)
	end


	app:add_camera("TrackBallCamera", "my_trackballus")
	app:add_camera("TrackBallCamera", "my_trackballus2")
	app:remove_camera("my_trackballus")

end

local val = 0
local xx = 0
local was_clicked = false
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

	if imgui.Button("screeeeen") then 
		app.screenshot("test0.png")
		app.screenshot("test1.png", 400)
	end

	if imgui.Button("lines!") then
		was_clicked = true
		local lr = app:get_renderer("line_renderer"):as("LineRenderer") -- TODO not have to make conversion
		-- local lr = app:get_renderer("line_renderer") -- TODO not have to make conversion
		lr:clear_lines()
		for i=1,1000 do

			
			local l = Line{
				a = vec3{i * 0.1, 0, 0},
				b = vec3{i * 0.1 + 0.09, 0, 0},
				color = vec3{1,i / 1000.0,0}
			}

			lr:add_line(l)
			
		end
		xx = xx + 1
		lr:push()

		app.camera.far_plane = 1000.
		app.camera:look_at_box({vec3{0,-5,-50}, vec3{100,5,50}})

	end

	if imgui.Button("points !!!") then 
		if app:has_renderer("my_point_set_renderer") then 
			local ps = app:get_renderer("my_point_set_renderer"):as("PointSetRenderer")
			for i=1,1000 do 
				ps:add_point(vec3{i*0.3,0,0})
			end
			ps.light = true
			ps.size = 10.
			ps.color = vec3{0.4, 0.2, 0.76}
			ps:push()
			app.camera.far_plane = 1000.
			app.camera:look_at_box({vec3{0,-5,-50}, vec3{100,5,50}})
		end
	end

	if imgui.Button("move points...") then 
		local ps = app:get_renderer("my_point_set_renderer"):as("PointSetRenderer")

		for i=1,1000 do 
			-- print(ps[i]:to_string())
			ps[i] = ps[i] + umvec3{0.1, 0, 0}
		end
		ps:push()
	end

	if imgui.Button("GOGOGO") then 
		app.navigation_path = {"diagnostic", "overlapping_vertices"}
	end

	imgui.Text("xx: " .. tostring(xx))

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