
function init()
	print("Hellow ! Script B loaded !")
end

local clipping_planes = {"x", "y", "z"}
local sel_clipping_plane = 1
local invert_clipping = false

function draw_gui()

	imgui.Begin("Mesh view options lua script")


	local fov, sw, sh = app.camera.GetFovAndScreen();
	local zoom_factor = -(fov - 45.) / (45. - 0.25) * 100.
	local str_zoom_factor = string.format("%.0f", zoom_factor)
	imgui.Text("Zoom: " .. str_zoom_factor .. "%")

	if (imgui.Button("Reset zoom")) then
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


	if (imgui.BeginCombo("##Clipping plane normal", clipping_planes[sel_clipping_plane])) then
		for i = 1, #clipping_planes do
			local is_selected = i == sel_clipping_plane
			if (imgui.Selectable(clipping_planes[i], is_selected)) then
				
				sel_clipping_plane = i

				if (i == 1) then
					app.renderer.setClippingPlaneNormal(1,0,0)
				elseif (i == 2) then
					app.renderer.setClippingPlaneNormal(0,1,0)
				elseif (i == 3) then
					app.renderer.setClippingPlaneNormal(0,0,1)
				end

			end
		end
		imgui.EndCombo()
	end


	local cpx, cpy, cpz = app.renderer.getClippingPlanePoint()
	local cnx, cny, cnz = app.renderer.getClippingPlaneNormal()

	local sel_slider_clipping_plane_point, new_clipping_plane_point = imgui.SliderFloat("Clipping plane point", cpx, -1., 1.)
	if (sel_slider_clipping_plane_point) then 
		app.renderer.setClippingPlanePoint(new_clipping_plane_point, cpy, cpz)
	end

	local sel_invert_clipping, new_invert_clipping = imgui.Checkbox("Invert clipping", invert_clipping)
	if (sel_invert_clipping) then 
		invert_clipping = new_invert_clipping
		app.renderer.invert_clipping = invert_clipping
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