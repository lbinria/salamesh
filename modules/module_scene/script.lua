function init()
	print("Load scene module")
	print("Number of models: " .. tostring(#app.models))
end

function draw_tree(model, k)
	if (imgui.TreeNode(k)) then 

		if (app.selected_model == k) then
			imgui.TextColored(1, 1, 0, 1, "Selected")
		end

		local model_pos = model.world_position

		local sel_visible, new_visible = imgui.Checkbox("Visible##" .. k .. "_visible", model.visible)
		if (sel_visible) then 
			model.visible = new_visible
		end


		imgui.SameLine()
		if (imgui.SmallButton("View")) then 
			app.selected_model = k
			-- Set camera position !
			-- app.camera.position = vec3.new(model_pos.x, model_pos.y, model_pos.z - 3.)
			-- app.camera.look_at = vec3.new(model_pos.x, model_pos.y, model_pos.z)
			app.camera:look_at_box(model.bbox)
		end

		draw_model_properties(model, k, 0)

		imgui.Separator()

		for k, child in pairs(app.models) do
			-- TODO ImGuiTreeNodeFlags_Selected if model selected
			if (child.parent == model) then 
				draw_tree(child, k)
			end
		end

		imgui.TreePop()
	end 
end

local clipping_planes = {"x", "y", "z"}
local sel_clipping_plane = {}

local invert_clipping = false

function draw_model_properties(model, k, view)
	local model_pos = model.world_position
	local p = model.position
	-- imgui.Text("world position: (%.4f, %.4f, %.4f)", model_pos.x, model_pos.y, model_pos.z);
	-- imgui.Text("local position: (%.4f, %.4f, %.4f)", p.x, p.y, p.z);
	-- TODO using ':' instead of '.' for to_string call... for sending self...
	imgui.Text("World position: " .. model_pos:to_string());
	imgui.Text("Local position: " .. p:to_string());
	imgui.Text("Center: " .. model.center:to_string());
	imgui.Text("Radius: " .. string.format("%.4f", model.radius));

	-- imgui.Text("Bounding box: " .. model.bbox[1]:to_string());
	imgui.Text("Number of vertices: " .. tostring(model.nverts));
	imgui.Text("Number of facets: " .. tostring(model.nfacets));


	-- if (imgui.CollapsingHeader("Properties##" .. k .. "_properties")) then 


		if (imgui.CollapsingHeader("Light##" .. k .. "_properties_light")) then 

			local sel_chk_enable_light, new_enable_light = imgui.Checkbox("Enable light", model.light)

			if (sel_chk_enable_light) then 
				print("Enable light: " .. tostring(new_enable_light))
				model.light = new_enable_light
			end

			local sel_chk_light_follow_view, new_light_follow_view = imgui.Checkbox("Light follow view", model.is_light_follow_view)

			if (sel_chk_light_follow_view) then 
				print("Enable light: " .. tostring(new_light_follow_view))
				model.is_light_follow_view = new_light_follow_view
			end

		end

		if (imgui.CollapsingHeader("Clipping##" .. k .. "_properties_clipping")) then 


			local sel_chk_enable_clipping, new_enable_clipping = imgui.Checkbox("Enable clipping", model.clipping)

			if (sel_chk_enable_clipping) then 
				print("Enable clipping: " .. tostring(new_enable_clipping))
				model.clipping = new_enable_clipping
				app.cull = false
			end

			if (imgui.BeginCombo("##Clipping plane mode", model.clipping_mode_strings[model.clipping_mode])) then
				for i = 1, #model.clipping_mode_strings do
					local is_selected = i == model.clipping_mode
					if (imgui.Selectable(model.clipping_mode_strings[i], is_selected)) then
						
						model.clipping_mode = i

						print("Set clipping mode to: " .. tostring(model.clipping_mode))
					end
				end
				imgui.EndCombo()
			end

			if not sel_clipping_plane[k] then 
				sel_clipping_plane[k] = 1
			end

			if (imgui.BeginCombo("##Clipping plane normal", clipping_planes[sel_clipping_plane[k]])) then
				for i = 1, #clipping_planes do
					local is_selected = i == sel_clipping_plane[k]
					if (imgui.Selectable(clipping_planes[i], is_selected)) then
						
						sel_clipping_plane[k] = i

						if (i == 1) then
							model.clipping_plane_normal = vec3.new(1,0,0)
						elseif (i == 2) then
							model.clipping_plane_normal = vec3.new(0,1,0)
						elseif (i == 3) then
							model.clipping_plane_normal = vec3.new(0,0,1)
						end

						print("Set clipping plane normal to: " .. model.clipping_plane_normal:to_string())
					end
				end
				imgui.EndCombo()
			end

			local plane_pos = 0
			-- Maybe it exists a better way to get model center from x,y,z than that ugly switch
			local center_at = 0;
			if (sel_clipping_plane[k] == 1) then
				plane_pos = model.clipping_plane_point.x
				center_at = model.center.x
			elseif (sel_clipping_plane[k] == 2) then
				plane_pos = model.clipping_plane_point.y
				center_at = model.center.y
			elseif (sel_clipping_plane[k] == 3) then
				plane_pos = model.clipping_plane_point.z
				center_at = model.center.z
			end

			-- 0 -> model.center - model.radius
			-- 1 -> model.center + model.radius
			local plane_pos_factor = (plane_pos - center_at) / model.radius

			local sel_slider_clipping_plane_point, new_clipping_plane_pos_factor = imgui.SliderFloat("Clipping plane point", plane_pos_factor, -1., 1.)
			if (sel_slider_clipping_plane_point) then 

				-- local new_clipping_plane_pos = model.radius * new_clipping_plane_pos_factor + center_at
				local new_clipping_plane_pos = center_at + new_clipping_plane_pos_factor * model.radius

				local v 
				if (sel_clipping_plane[k] == 1) then
					v = vec3.new(new_clipping_plane_pos, model.clipping_plane_point.y, model.clipping_plane_point.z)
				elseif (sel_clipping_plane[k] == 2) then
					v = vec3.new(model.clipping_plane_point.x, new_clipping_plane_pos, model.clipping_plane_point.z)
				elseif (sel_clipping_plane[k] == 3) then
					v = vec3.new(model.clipping_plane_point.x, model.clipping_plane_point.y, new_clipping_plane_pos)
				end
				model.clipping_plane_point = v
			end

			local sel_invert_clipping, new_invert_clipping = imgui.Checkbox("Invert clipping", invert_clipping)
			if (sel_invert_clipping) then 
				invert_clipping = new_invert_clipping
				model.invert_clipping = invert_clipping
			end

		end

		if (imgui.CollapsingHeader("Style##" .. k .. "_properties_style")) then 

			if model.mesh then

				local sel_mesh_visible, new_mesh_visible = imgui.Checkbox("Show mesh", model.mesh.visible)
				if (sel_mesh_visible) then 
					print("Change mesh visibility: " .. tostring(new_mesh_visible))
					model.mesh.visible = new_mesh_visible
				end

				local sel_color, new_color = imgui.ColorEdit3("Color", model.mesh.color)
				if (sel_color) then 
					print("Change color: " .. tostring(new_color))
					model.mesh.color = new_color
				end

				local sel_slider_mesh_size, new_mesh_size = imgui.SliderFloat("Mesh size", model.mesh.size, 0, 20)
				if (sel_slider_mesh_size) then 
					print("Change mesh size: " .. tostring(new_mesh_size))
					model.mesh.size = new_mesh_size
				end

				local sel_slider_mesh_shrink, new_mesh_shrink = imgui.SliderFloat("Mesh shrink", model.mesh.shrink, 0, 1)
				if (sel_slider_mesh_shrink) then 
					print("Change mesh shrink: " .. tostring(new_mesh_shrink))
					model.mesh.shrink = new_mesh_shrink
				end

				local sel_corner_visible, new_corner_visible = imgui.Checkbox("Show corners", model.mesh.corner_visible)
				if (sel_corner_visible) then 
					print("Change corner visibility: " .. tostring(new_corner_visible))
					model.mesh.corner_visible = new_corner_visible
				end

			end

			local sel_point_visible, new_point_visible = imgui.Checkbox("Show points", model.points.visible)
			if (sel_point_visible) then 
				print("Change point visibility: " .. tostring(new_point_visible))
				model.points.visible = new_point_visible
			end

			local sel_point_size, new_point_size = imgui.SliderFloat("Point size", model.points.size, 0, 50)
			if (sel_point_size) then 
				print("Change point size: " .. tostring(new_point_size))
				model.points.size = new_point_size
			end

			local sel_point_color, new_point_color = imgui.ColorEdit3("Point color", model.points.color)
			if (sel_point_color) then 
				-- print("Change point color: " .. tostring(new_point_color))
				model.points.color = new_point_color
			end

			if model.edges then 

				local sel_edge_visible, new_edge_visible = imgui.Checkbox("Show edges", model.edges.visible)
				if (sel_edge_visible) then 
					print("Change edge visibility: " .. tostring(new_edge_visible))
					model.edges.visible = new_edge_visible
				end

				local sel_edge_thickness, new_edge_thickness = imgui.SliderFloat("Edge thickness", model.edges.thickness, 0, 50)
				if (sel_edge_thickness) then 
					print("Change edge thickness: " .. tostring(new_edge_thickness))
					model.edges.thickness = new_edge_thickness
				end

				local sel_edge_spacing, new_edge_spacing = imgui.SliderFloat("Edge spacing", model.edges.spacing, 0, 1)
				if (sel_edge_spacing) then 
					print("Change edge spacing: " .. tostring(new_edge_spacing))
					model.edges.spacing = new_edge_spacing
				end

				local sel_edge_padding, new_edge_padding = imgui.SliderFloat("Edge padding", model.edges.padding, 0, 1)
				if (sel_edge_padding) then 
					print("Change edge padding: " .. tostring(new_edge_padding))
					model.edges.padding = new_edge_padding
				end

				local sel_edge_inside_color, new_edge_inside_color = imgui.ColorEdit3("Edge inside color", model.edges.inside_color)
				if (sel_edge_inside_color) then 
					print("Change edge inside color: " .. tostring(new_edge_inside_color))
					model.edges.inside_color = new_edge_inside_color
				end

				local sel_edge_outside_color, new_edge_outside_color = imgui.ColorEdit3("Edge outside color", model.edges.outside_color)
				if (sel_edge_outside_color) then 
					print("Change edge outside color: " .. tostring(new_edge_outside_color))
					model.edges.outside_color = new_edge_outside_color
				end
			end

		end

		if (imgui.CollapsingHeader("Attributes##" .. k .. "_properties_attributes")) then 

			imgui.Text("Colormap 0")

			local colormaps = app.colormaps
			local items = {}
			for i = 1, #colormaps do 
				table.insert(items, colormaps[i].name)
			end

			local colormap_size = imgui.ImVec2(320, 35)

			if (imgui.BeginCombo("##combo_colormaps0_selection", items[model.selected_colormap0])) then
				-- Display items in the popup
				for i = 1, #items do
					local is_selected = model.selected_colormap0 == i
					-- Create a unique ID for each item to prevent conflicts
					imgui.PushID(i)

					-- Calculate total width including spacing
					-- local total_width = imgui.CalcTextSize(items[i]).x + colormap_size.x + 10.0

					-- Display the item with both text and image
					if (imgui.Selectable(items[i], is_selected)) then
						model.selected_colormap0 = i
					end

					-- Display the image after the text
					imgui.Image(app.colormaps[i].tex, colormap_size)

					imgui.PopID()
				end

				imgui.EndCombo()
			end

			local selected_cm = app.colormaps[model.selected_colormap0]
			if selected_cm.height > 1 then 
				local h = selected_cm.height / selected_cm.width * 320
				colormap_size = imgui.ImVec2(320, h)
			end

			imgui.Image(
				selected_cm.tex, 
				colormap_size
			)

			imgui.Text("Colormap 1")

			local colormaps = app.colormaps
			local items = {}
			for i = 1, #colormaps do 
				table.insert(items, colormaps[i].name)
			end
			local colormap_size = imgui.ImVec2(320, 35)

			if (imgui.BeginCombo("##combo_colormaps1_selection", items[model.selected_colormap1])) then
				-- Display items in the popup
				for i = 1, #items do
					local is_selected = model.selected_colormap1 == i
					-- Create a unique ID for each item to prevent conflicts
					imgui.PushID(i)

					-- Calculate total width including spacing
					-- local total_width = imgui.CalcTextSize(items[i]).x + colormap_size.x + 10.0

					-- Display the item with both text and image
					if (imgui.Selectable(items[i], is_selected)) then
						model.selected_colormap1 = i
					end

					-- Display the image after the text
					imgui.Image(app.colormaps[i].tex, colormap_size)

					imgui.PopID()
				end

				imgui.EndCombo()
			end

			local selected_cm = app.colormaps[model.selected_colormap1]
			if selected_cm.height > 1 then 
				local h = selected_cm.height / selected_cm.width * 320
				colormap_size = imgui.ImVec2(320, h)
			end

			imgui.Image(
				selected_cm.tex, 
				colormap_size
			)

			imgui.Text("Attribute 0")

			if (#model.attrs > 0) then
				-- local attr_name, attr_element = model.attrs[1]
				local attr_name = model.attrs[1].name
				local attr_element = model.attrs[1].kind
				-- local attr_name, attr_element = model.get_attr(1);
				-- print("first attr:" .. attr_name)
				-- print("second attr:" .. attr_element)
				
				local selName = "None" 
				if model.selected_attr0 > 0 then 
					selName = model.attrs[model.selected_attr0].name
				end

				if (imgui.BeginCombo("##combo_attribute0_selection", selName)) then

					local is_selected = model.selected_attr0 == 0
					if (imgui.Selectable("None", is_selected)) then
						model.selected_attr0 = 0
					end

					for n = 1, #model.attrs do
						local is_selected = n == model.selected_attr0
						local label = model.attrs[n].name 
						-- .. "[" .. tostring(model.attrs[n].selected_dim) .. "]"
						.. " (" .. element_kind_to_string(model.attrs[n].kind) .. ")" 
						.. " (" .. element_type_to_string(model.attrs[n].type) .. ")"
						.. " (" .. tostring(model.attrs[n].dim) .. ")"

						if (imgui.Selectable(label, is_selected)) then
							model.selected_attr0 = n

							-- print("set attr: " .. model.attrs[n][1] .. ":" .. model.attrs[n][2] .. ":" .. model.attrs[n][3])
						end
					end
					imgui.EndCombo()
				end
			end

			imgui.Text("Attribute 1")

			if (#model.attrs > 0) then
				-- local attr_name, attr_element = model.attrs[1]
				local attr_name = model.attrs[1].name
				local attr_element = model.attrs[1].kind
				-- local attr_name, attr_element = model.get_attr(1);
				-- print("first attr:" .. attr_name)
				-- print("second attr:" .. attr_element)

				local selName = "None" 
				if model.selected_attr1 > 0 then 
					selName = model.attrs[model.selected_attr1].name
				end

				if (imgui.BeginCombo("##combo_attribute1_selection", selName)) then

					local is_selected = model.selected_attr1 == 0
					if (imgui.Selectable("None", is_selected)) then
						model.selected_attr1 = 0
					end

					for n = 1, #model.attrs do
						local is_selected = n == model.selected_attr1
						local label = model.attrs[n].name 
						.. " (" .. element_kind_to_string(model.attrs[n].kind) .. ")" 
						.. " (" .. element_type_to_string(model.attrs[n].type) .. ")"
						.. " (" .. tostring(model.attrs[n].dim) .. ")"

						if (imgui.Selectable(label, is_selected)) then
							model.selected_attr1 = n

							-- print("set attr: " .. model.attrs[n][1] .. ":" .. model.attrs[n][2] .. ":" .. model.attrs[n][3])
						end
					end
					imgui.EndCombo()
				end
			end

		end
	-- end
end

function layout_gui() 
	return {["Scene"] = "nav_bar"}
end

function draw_gui()
	imgui.Begin("Scene")

	if (imgui.BeginTabBar("Scene Tabs")) then 

		if (imgui.BeginTabItem("Flat view")) then

			for k, model in pairs(app.models) do
				
				local sel_visible, new_visible = imgui.Checkbox(k .. "##" .. k, model.visible)
				if (sel_visible) then 
					model.visible = new_visible
				end

				imgui.SameLine()
				if (imgui.Button("View##" .. "btn_view_" .. k)) then
					app.selected_model = k
					-- Set camera position !
					local model_pos = model.center
					-- app.camera.position = vec3.new(model_pos.x, model_pos.y, model_pos.z - model.radius * 2.);
					-- app.camera.look_at = vec3.new(model_pos.x, model_pos.y, model_pos.z);
					app.camera:look_at_box(model.bbox)
				end
				
			end

			imgui.Separator()

			-- Check if app has at least one model, to draw properties of current one (if exists)
			if (app.has_models) then
				draw_model_properties(app.model, app.selected_model, 0)
			end

			imgui.EndTabItem()

		end 

		if (imgui.BeginTabItem("Hierarchical view")) then

			-- Scene graph
			imgui.Separator()
			if (imgui.TreeNode("Scene##tree_node_scene")) then 

				-- for k, model in app.models do 
				for k, model in pairs(app.models) do
					-- TODO ImGuiTreeNodeFlags_Selected if model selected
					if (model.parent == nil) then 
						draw_tree(model, k)
					end
				end

				imgui.TreePop()
			end

			
			imgui.EndTabItem()
		end

		if (imgui.BeginTabItem("Cameras")) then

			imgui.Text("Cameras")
			imgui.Separator()

			if (imgui.BeginListBox("##list_box_cameras")) then
				for k, _ in pairs(app.cameras) do

					local is_selected = k == app.selected_camera

					-- Add unique id to prevent conflicts
					if (imgui.Selectable(k .. "##list_box_selectable_" .. k, is_selected)) then
						app.selected_camera = k
					end
					
				end
				imgui.EndListBox()
			end
			
			imgui.Text("Properties")
			imgui.Separator()
			local camera = app.camera
			local p = camera.position

			-- Compute zoom factor (varies between 0.001 for 0% - 1 for 100%)
			-- local fov = app.camera.fov
			-- local zoom_factor = -(fov - 45.) / (45. - 0.25) * 100.
			local zoom_factor = app.camera.zoom * 100.
			
			
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