function init()
	print("Load scene module")
	print("Number of models: " .. tostring(#app.models))
end

function draw_tree(model, i)
	if (imgui.TreeNode(model.name)) then 

		if (app.selected_model == i) then
			imgui.TextColored(1, 1, 0, 1, "Selected")
		end

		local model_pos = model.world_position

		local sel_visible, new_visible = imgui.Checkbox("Visible##" .. model.name .. "_visible", model.visible)
		if (sel_visible) then 
			model.visible = new_visible
		end


		imgui.SameLine()
		if (imgui.SmallButton("View")) then 
			app.selected_model = i
			-- Set camera position !
			app.camera.position = vec3.new(model_pos.x, model_pos.y, model_pos.z - 3.);
			app.camera.look_at = vec3.new(model_pos.x, model_pos.y, model_pos.z);
		end

		draw_model_properties(model, 0)

		imgui.Separator()

		for i = 1, #app.models do
			-- TODO ImGuiTreeNodeFlags_Selected if model selected
			local child = app.models[i]
			if (child.parent ~= model) then 
				goto continue
			end

			draw_tree(child, i)
			::continue::
		end

		imgui.TreePop()
	end 
end

local clipping_planes = {"x", "y", "z"}
local sel_clipping_plane = 1
local invert_clipping = false

function draw_model_properties(model, view)
	local model_pos = model.world_position
	local p = model.position
	-- imgui.Text("world position: (%.4f, %.4f, %.4f)", model_pos.x, model_pos.y, model_pos.z);
	-- imgui.Text("local position: (%.4f, %.4f, %.4f)", p.x, p.y, p.z);
	-- TODO using ':' instead of '.' for to_string call... for sending self...
	imgui.Text("World position: " .. model_pos:to_string());
	imgui.Text("Local position: " .. p:to_string());


	-- local cur_model = app.model
	local cur_model = model

	if (imgui.CollapsingHeader("Properties##" .. cur_model.name .. "_properties")) then 


		if (imgui.CollapsingHeader("Light##" .. cur_model.name .. "_properties_light")) then 

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

		end

		if (imgui.CollapsingHeader("Clipping##" .. cur_model.name .. "_properties_clipping")) then 


			local sel_chk_enable_clipping, new_enable_clipping = imgui.Checkbox("Enable clipping", cur_model.clipping)

			if (sel_chk_enable_clipping) then 
				print("Enable clipping: " .. tostring(new_enable_clipping))
				cur_model.clipping = new_enable_clipping
				app.cull = false
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

		end

		if (imgui.CollapsingHeader("Style##" .. cur_model.name .. "_properties_style")) then 

			local sel_mesh_visible, new_mesh_visible = imgui.Checkbox("Show mesh", cur_model.mesh_visible)
			if (sel_mesh_visible) then 
				print("Change mesh visibility: " .. tostring(new_mesh_visible))
				cur_model.mesh_visible = new_mesh_visible
			end

			local sel_color, new_color = imgui.ColorEdit3("Color", cur_model.color)
			if (sel_color) then 
				print("Change color: " .. tostring(new_color))
				cur_model.color = new_color
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

			local sel_point_visible, new_point_visible = imgui.Checkbox("Show points", cur_model.point_visible)
			if (sel_point_visible) then 
				print("Change point visibility: " .. tostring(new_point_visible))
				cur_model.point_visible = new_point_visible
			end

			local sel_point_size, new_point_size = imgui.SliderFloat("Point size", cur_model.point_size, 0, 50)
			if (sel_point_size) then 
				print("Change point size: " .. tostring(new_point_size))
				cur_model.point_size = new_point_size
			end

			local sel_point_color, new_point_color = imgui.ColorEdit3("Point color", cur_model.point_color)
			if (sel_point_color) then 
				-- print("Change point color: " .. tostring(new_point_color))
				cur_model.point_color = new_point_color
			end

			local sel_edge_visible, new_edge_visible = imgui.Checkbox("Show edge", cur_model.edge_visible)
			if (sel_edge_visible) then 
				print("Change edge visibility: " .. tostring(new_edge_visible))
				cur_model.edge_visible = new_edge_visible
			end

			local sel_edge_size, new_edge_size = imgui.SliderFloat("Edge size", cur_model.edge_size, 0, 50)
			if (sel_edge_size) then 
				print("Change edge size: " .. tostring(new_edge_size))
				cur_model.edge_size = new_edge_size
			end

			local sel_edge_inside_color, new_edge_inside_color = imgui.ColorEdit3("Edge inside color", cur_model.edge_inside_color)
			if (sel_edge_inside_color) then 
				print("Change edge inside color: " .. tostring(new_edge_inside_color))
				cur_model.edge_inside_color = new_edge_inside_color
			end

			local sel_edge_outside_color, new_edge_outside_color = imgui.ColorEdit3("Edge outside color", cur_model.edge_outside_color)
			if (sel_edge_outside_color) then 
				print("Change edge outside color: " .. tostring(new_edge_outside_color))
				cur_model.edge_outside_color = new_edge_outside_color
			end

		end

		-- if (imgui.BeginCombo("Pick Mode", app.pick_mode_strings[app.pick_mode + 1])) then
		-- 	for i = 1, #app.pick_mode_strings do
		-- 		local is_selected = (i - 1) == app.pick_mode
		-- 		if (imgui.Selectable(app.pick_mode_strings[i], is_selected)) then
		-- 			print("Change pick mode to: " .. app.pick_mode_strings[i])
		-- 			app.pick_mode = i - 1
		-- 		end
		-- 	end
		-- 	imgui.EndCombo()
		-- end

		if (imgui.CollapsingHeader("Attributes##" .. cur_model.name .. "_properties_attributes")) then 

			if (imgui.BeginCombo("##Color Mode", cur_model.color_mode_strings[cur_model.color_mode + 1])) then
				for i = 1, #cur_model.color_mode_strings do
					local is_selected = (i - 1) == app.pick_mode
					if (imgui.Selectable(cur_model.color_mode_strings[i], is_selected)) then
						print("Change color mode to: " .. cur_model.color_mode_strings[i])
						cur_model.color_mode = i - 1

						if (cur_model.color_mode == 1) then
							print("Change to attribute color mode")
							cur_model.selected_attr = cur_model.selected_attr
						end

					end
				end
				imgui.EndCombo()
			end

			if (cur_model.color_mode == 1) then
				local items = {"Item1", "Item2"}
				local colormap_size = imgui.ImVec2(320, 35)

				if (imgui.BeginCombo("##combo_colormaps_selection", items[cur_model.selected_colormap + 1])) then
					-- Display items in the popup
					for i = 1, #items do
						local is_selected = (cur_model.selected_colormap + 1) == i
						-- Create a unique ID for each item to prevent conflicts
						imgui.PushID(i)

						-- Calculate total width including spacing
						-- local total_width = imgui.CalcTextSize(items[i]).x + colormap_size.x + 10.0

						-- Display the item with both text and image
						if (imgui.Selectable(items[i], is_selected)) then
							cur_model.selected_colormap = i - 1
						end

						-- Display the image after the text
						imgui.Image(app.colormaps_2d[i], colormap_size)

						imgui.PopID()
					end

					imgui.EndCombo()
				end

				imgui.Image(
					app.colormaps_2d[cur_model.selected_colormap + 1], 
					colormap_size
				)

				imgui.Text("Attribute")

				if (#cur_model.attrs > 0) then
					-- local attr_name, attr_element = cur_model.attrs[1]
					local attr_name = cur_model.attrs[1].name
					local attr_element = cur_model.attrs[1].kind
					-- local attr_name, attr_element = cur_model.get_attr(1);
					-- print("first attr:" .. attr_name)
					-- print("second attr:" .. attr_element)
					if (imgui.BeginCombo("##combo_attribute_selection", cur_model.attrs[cur_model.selected_attr].name)) then
						for n = 1, #cur_model.attrs do
							local is_selected = n == cur_model.selected_attr
							local label = cur_model.attrs[n].name .. " (" .. elementKindToString(cur_model.attrs[n].kind) .. ")" .. " (" .. elementTypeToString(cur_model.attrs[n].type) .. ")"
							if (imgui.Selectable(label, is_selected)) then
								cur_model.selected_attr = n

								-- print("set attr: " .. cur_model.attrs[n][1] .. ":" .. cur_model.attrs[n][2] .. ":" .. cur_model.attrs[n][3])
							end
						end
						imgui.EndCombo()
					end
				end


			end 

		end
	end
end

function draw_gui()
	imgui.Begin("Scene")

	if (imgui.BeginTabBar("Scene Tabs")) then 

		if (imgui.BeginTabItem("Flat view")) then

			for i = 1, #app.models do
				local model = app.models[i]
				imgui.PushID(i)

				local sel_visible, new_visible = imgui.Checkbox(model.name .. "##" .. i, model.visible)
				if (sel_visible) then 
					model.visible = new_visible
				end

				imgui.SameLine()
				if (imgui.Button("View")) then
					app.selected_model = i
					-- Set camera position !
					local model_pos = model.world_position
					app.camera.position = vec3.new(model_pos.x, model_pos.y, model_pos.z - 3.);
					app.camera.look_at = vec3.new(model_pos.x, model_pos.y, model_pos.z);
				end
				imgui.PopID()
			end

			imgui.Separator()

			draw_model_properties(app.model, 0)

			imgui.EndTabItem()

		end 

		if (imgui.BeginTabItem("Hierarchical view")) then

			-- Scene graph
			imgui.Separator()
			if (imgui.TreeNode("Scene##tree_node_scene")) then 

				-- for k, model in app.models do 
				for i = 1, #app.models do
					-- TODO ImGuiTreeNodeFlags_Selected if model selected
					local model = app.models[i]
					if (model.parent ~= nil) then 
						goto continue
					end

					draw_tree(model, i)
					::continue::
				end

				imgui.TreePop()
			end

			
			imgui.EndTabItem()
		end

		if (imgui.BeginTabItem("Cameras")) then

			imgui.Text("Cameras")
			imgui.Separator()

			if (imgui.BeginListBox("##list_box_cameras")) then
				for i = 1, #app.cameras do

					local camera = app.cameras[i]

					local is_selected = i == app.selected_camera
					-- Create a unique ID for each item to prevent conflicts
					imgui.PushID(i)

					if (imgui.Selectable(tostring(i) .. ". " .. camera.name, is_selected)) then
						app.selected_camera = i
					end


					imgui.PopID()
					
				end
				imgui.EndListBox()
			end
			
			imgui.Text("Properties")
			imgui.Separator()
			local camera = app.camera
			local p = camera.position

			-- Compute zoom factor
			local fov = app.camera.fov
			local zoom_factor = -(fov - 45.) / (45. - 0.25) * 100.
			-- Convert -0 value to 0
			if (zoom_factor <= 0 and zoom_factor > -0.001) then zoom_factor = 0 end

			local str_zoom_factor = string.format("%.0f", zoom_factor)

			imgui.Text("Zoom: " .. str_zoom_factor .. "%")
			imgui.SameLine()
			if (imgui.SmallButton("Reset zoom")) then
				app.camera:reset_zoom()
			end

			imgui.Text("Position: " .. p:to_string());
			imgui.Text("Look at: " .. camera.look_at:to_string());

			imgui.EndTabItem()
		end

		imgui.EndTabBar()
	end



	imgui.End()

end