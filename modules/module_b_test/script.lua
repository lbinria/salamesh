
function init()
	print("Hellow ! Script B loaded !")
end

local clipping_planes = {"x", "y", "z"}
local sel_clipping_plane = 1
local invert_clipping = false

function draw_gui()

	imgui.Begin("Mesh view options lua script")

	if (imgui.Button("Cull back")) then 
		print("Button cull back pressed")
		app.setCullMode(0x0405)
	end

	if (imgui.Button("Cull front")) then 
		print("Button cull front pressed")
		app.setCullMode(0x0404)
	end

	local cur_model = app.current_model

	local sel_chk_enable_light, new_enable_light = imgui.Checkbox("Enable light", cur_model.light)

	if (sel_chk_enable_light) then 
		print("Enable light: " .. tostring(new_enable_light))
		cur_model.light = new_enable_light
	end

	local sel_chk_light_follow_view, new_light_follow_view = imgui.Checkbox("Light follow view", cur_model.is_light_follow_view)

	if (sel_chk_light_follow_view) then 
		print("Enable light: " .. tostring(new_light_follow_view))
		cur_model.is_light_follow_view = new_light_follow_view
	end

	local sel_chk_enable_clipping, new_enable_clipping = imgui.Checkbox("Enable clipping", cur_model.clipping)

	if (sel_chk_enable_clipping) then 
		print("Enable clipping: " .. tostring(new_enable_clipping))
		cur_model.clipping = new_enable_clipping
	end

	if (imgui.BeginCombo("##Clipping plane normal", clipping_planes[sel_clipping_plane])) then
		for i = 1, #clipping_planes do
			local is_selected = i == sel_clipping_plane
			if (imgui.Selectable(clipping_planes[i], is_selected)) then
				
				sel_clipping_plane = i

				if (i == 1) then
					cur_model.clipping_plane_normal = vec3.new(1,0,0)
				elseif (i == 2) then
					cur_model.clipping_plane_normal = vec3.new(0,1,0)
				elseif (i == 3) then
					cur_model.clipping_plane_normal = vec3.new(0,0,1)
				end

				print("Set clipping plane normal to: " .. cur_model.clipping_plane_normal:to_string())
			end
		end
		imgui.EndCombo()
	end

	local plane_pos = 0
	if (sel_clipping_plane == 1) then
		plane_pos = cur_model.clipping_plane_point.x
	elseif (sel_clipping_plane == 2) then
		plane_pos = cur_model.clipping_plane_point.y
	elseif (sel_clipping_plane == 3) then
		plane_pos = cur_model.clipping_plane_point.z
	end

	local sel_slider_clipping_plane_point, new_clipping_plane_pos = imgui.SliderFloat("Clipping plane point", plane_pos, -1., 1.)
	if (sel_slider_clipping_plane_point) then 
		local v 
		if (sel_clipping_plane == 1) then
			v = vec3.new(new_clipping_plane_pos, cur_model.clipping_plane_point.y, cur_model.clipping_plane_point.z)
		elseif (sel_clipping_plane == 2) then
			v = vec3.new(cur_model.clipping_plane_point.x, new_clipping_plane_pos, cur_model.clipping_plane_point.z)
		elseif (sel_clipping_plane == 3) then
			v = vec3.new(cur_model.clipping_plane_point.x, cur_model.clipping_plane_point.y, new_clipping_plane_pos)
		end
		cur_model.clipping_plane_point = v
	end

	local sel_invert_clipping, new_invert_clipping = imgui.Checkbox("Invert clipping", invert_clipping)
	if (sel_invert_clipping) then 
		invert_clipping = new_invert_clipping
		cur_model.invert_clipping = invert_clipping
	end


	local sel_slider_mesh_size, new_mesh_size = imgui.SliderFloat("Mesh size", cur_model.meshSize, 0, 1)
	if (sel_slider_mesh_size) then 
		print("Change mesh size: " .. tostring(new_mesh_size))
		cur_model.meshSize = new_mesh_size
	end

	local sel_slider_mesh_shrink, new_mesh_shrink = imgui.SliderFloat("Mesh shrink", cur_model.meshShrink, 0, 1)
	if (sel_slider_mesh_shrink) then 
		print("Change mesh shrink: " .. tostring(new_mesh_shrink))
		cur_model.meshShrink = new_mesh_shrink
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