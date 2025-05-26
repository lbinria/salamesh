function init()
	print("Hellow ! Script B loaded !")

	-- app.print_test()
	-- app.test = 1;
	-- app.print_test()
	-- app.test = 2;
	-- app.print_test()

	-- print(app.test)
end

-- function key_event(key, scancode, action, mods)
-- 	print("Key event: " .. key .. ", " .. scancode .. ", " .. action .. ", " .. mods)
-- 	if (key == 78) then 
-- 		app.reset_zoom();
-- 	end
-- end

-- TODO BIND enable_light, enable_clipping, etc variables to app variable, not lua ones
local enable_light = false
local enable_clipping = false
local mesh_size = 0.0
local mesh_shrink = 0.0

function draw_gui()
	imgui.Begin("Mesh view options lua script")

	-- imgui.Text("Zoom: %.0f%%", "")
	imgui.Text("Zoom: " .. "0" .. "%")

	if (imgui.Button("Cull back")) then 
		print("Button cull back pressed")
		app.setCullMode(0x0405)
	end

	if (imgui.Button("Cull front")) then 
		print("Button cull front pressed")
		app.setCullMode(0x0404)
	end

	local sel_chk_enable_light, new_enable_light = imgui.Checkbox("Enable light", enable_light)

	if (sel_chk_enable_light) then 
		print("Enable light: " .. tostring(new_enable_light))
		enable_light = new_enable_light
		-- app.setLight(enable_light);
		app.renderer.setLight(enable_light);
	end

	local sel_chk_enable_clipping, new_enable_clipping = imgui.Checkbox("Enable clipping", enable_clipping)

	if (sel_chk_enable_clipping) then 
		print("Enable clipping: " .. tostring(new_enable_clipping))
		enable_clipping = new_enable_clipping
		-- app.setClipping(enable_clipping);
		app.renderer.setClipping(enable_clipping);
	end

	local sel_slider_mesh_size, new_mesh_size = imgui.SliderFloat("Mesh size", mesh_size, 0, 1)
	if (sel_slider_mesh_size) then 
		print("Change mesh size: " .. tostring(new_mesh_size))
		mesh_size = new_mesh_size
		app.renderer.setMeshSize(mesh_size)
	end

	local sel_slider_mesh_shrink, new_mesh_shrink = imgui.SliderFloat("Mesh shrink", mesh_shrink, 0, 1)
	if (sel_slider_mesh_shrink) then 
		print("Change mesh size: " .. tostring(new_mesh_shrink))
		mesh_shrink = new_mesh_shrink
		app.renderer.setMeshShrink(mesh_shrink)
	end

	imgui.End()

end