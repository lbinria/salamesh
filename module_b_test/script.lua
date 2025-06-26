
function init()
	print("Hellow ! Script B loaded !")
end

function draw_gui()

	imgui.Begin("Mesh view options lua script")


	local fov, sw, sh = app.camera.GetFovAndScreen();
	local zoom_factor = -(fov - 45.) / (45. - 0.25) * 100.
	local str_zoom_factor = string.format("%.0f", zoom_factor)
	imgui.Text("Zoom: " .. str_zoom_factor .. "%")

	if (imgui.Button("Re²set zoom")) then
		app.reset_zoom()
	end

	if (imgui.Button("Look at center")) then 
		app.look_at_center()
	end

	if (imgui.Button("Cull back")) then 
		print("Button cull back pressed")
		app.setCullMode(0x0405)
	end

	if (imgui.Button("Cull front")) then 
		print("Button cull front pressed")
		app.setCullMode(0x0404)
	end

	local sel_chk_enable_light, new_enable_light = imgui.Checkbox("Enable light", app.renderer.light)

	if (sel_chk_enable_light) then 
		print("Enable light: " .. tostring(new_enable_light))
		app.renderer.light = new_enable_light
	end

	local sel_chk_enable_clipping, new_enable_clipping = imgui.Checkbox("Enable clipping", app.renderer.clipping)

	if (sel_chk_enable_clipping) then 
		print("Enable clipping: " .. tostring(new_enable_clipping))
		app.renderer.clipping = new_enable_clipping
	end

	local sel_slider_mesh_size, new_mesh_size = imgui.SliderFloat("Mesh size", app.renderer.meshSize, 0, 1)
	if (sel_slider_mesh_size) then 
		print("Change mesh size: " .. tostring(new_mesh_size))
		app.renderer.meshSize = new_mesh_size
	end

	local sel_slider_mesh_shrink, new_mesh_shrink = imgui.SliderFloat("Mesh shrink", app.renderer.meshShrink, 0, 1)
	if (sel_slider_mesh_shrink) then 
		print("Change mesh shrink: " .. tostring(new_mesh_shrink))
		app.renderer.meshShrink = new_mesh_shrink
	end

	if (imgui.BeginCombo("Pick Mode", app.pick_mode_strings[app.pick_mode + 1])) then
		for i = 1, #app.pick_mode_strings do
			local is_selected = (i - 1) == app.pick_mode
			if (imgui.Selectable(app.pick_mode_strings[i], is_selected)) then
				print("Change pick mode to: " .. app.pick_mode_strings[i])
				app.pick_mode = i - 1
			end
		end
		imgui.EndCombo()
	end

	imgui.End()

end