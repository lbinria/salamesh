function init()
	print("Load scene module")
	print("Number of models: " .. tostring(#app.scene.models))
end

local clipping_planes = {"x", "y", "z"}
local sel_clipping_plane = {}
local invert_clipping = false

-- Selected colormap index by model (model model)
local selected_colormaps = {}
-- Selected attribute index by model (model model)
local selected_attributes = {}

function draw_model_properties(model, k, view)
	local p = model.position

	if (imgui.CollapsingHeader("Infos##" .. k .. "_infos")) then 

		imgui.Text("Position: " .. p:to_string());
		imgui.Text("Center: " .. model.mesh.center:to_string());
		imgui.Text("Radius: " .. string.format("%.4f", model.mesh.radius));
		-- imgui.Text("Bounding box: " .. model.bbox[1]:to_string());
		imgui.Text("Number of vertices: " .. tostring(model.mesh.nverts));
		imgui.Text("Number of facets: " .. tostring(model.mesh.nfacets));

	end
	-- if (imgui.CollapsingHeader("Properties##" .. k .. "_properties")) then 


		if (imgui.CollapsingHeader("Light##" .. k .. "_properties_light")) then 

			local light = false
			for _, material in pairs(model.materials) do
				if (material["light"]["enabled"]) then 
					light = true
				end
			end

			local sel_chk_enable_light, new_enable_light = imgui.Checkbox("Enable light", light)

			if (sel_chk_enable_light) then 
				print("Enable light: " .. tostring(new_enable_light))
				-- model.light = new_enable_light
				
				for _, material in pairs(model.materials) do
					material["light"]["enabled"] = new_enable_light
				end

			end

			local light_dir
			for _, material in pairs(model.materials) do
				light_dir = material["light"]["dir"]
				break
			end

			local sel_chk_light_dir, new_light_dir = imgui.DragFloat3("light dir", light_dir, 0.01, -1., 1.)

			if (sel_chk_light_dir) then 
				for _, material in pairs(model.materials) do
					material["light"]["dir"] = new_light_dir
				end
			end

		end

		if (imgui.CollapsingHeader("Clipping##" .. k .. "_properties_clipping")) then

			local clip = false
			local clipping_mode = ClippingMode.CELL
			for _, material in pairs(model.materials) do
				if (material["clipping"]["enabled"]) then 
					clip = true
				end
				if (material["clipping"]) then 
					clipping_mode = material["clipping"]["mode"]
				end
			end


			local sel_chk_enable_clipping, new_enable_clipping = imgui.Checkbox("Enable clipping", clip)

			if (sel_chk_enable_clipping) then 
				print("Enable clipping: " .. tostring(new_enable_clipping))

				for _, material in pairs(model.materials) do
					material["clipping"]["enabled"] = new_enable_clipping
				end
		
			end

			if (imgui.BeginCombo("##Clipping plane mode", ClippingInfo.clipping_mode_strings[clipping_mode + 1])) then
				for i = 1, #ClippingInfo.clipping_mode_strings do
					local is_selected = i - 1 == clipping_mode
					if (imgui.Selectable(ClippingInfo.clipping_mode_strings[i], is_selected)) then

						for _, material in pairs(model.materials) do
							material["clipping"]["mode"] = i - 1
						end

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

						local v = vec3{1,0,0}
						if (i == 2) then
							v = vec3{0,1,0}
						elseif (i == 3) then
							v = vec3{0,0,1}
						end

						for _, material in pairs(model.materials) do 
							material["clipping"]["normal"] = v
						end

						print("Set clipping plane normal to: " .. v:to_string())
					end
				end
				imgui.EndCombo()
			end

			-- Get clipping point (arbitrary the first found)
			local clipping_plane_point = vec3{0,0,0}
			for _, material in pairs(model.materials) do 
				clipping_plane_point = material["clipping"]["point"]
				break 
			end

			local plane_pos = 0
			-- Maybe it exists a better way to get model center from x,y,z than that ugly switch
			local center_at = 0;
			if (sel_clipping_plane[k] == 1) then
				plane_pos = clipping_plane_point.x
				center_at = model.mesh.center.x
			elseif (sel_clipping_plane[k] == 2) then
				plane_pos = clipping_plane_point.y
				center_at = model.mesh.center.y
			elseif (sel_clipping_plane[k] == 3) then
				plane_pos = clipping_plane_point.z
				center_at = model.mesh.center.z
			end

			-- 0 -> model.center - model.radius
			-- 1 -> model.center + model.radius
			local plane_pos_factor = (plane_pos - center_at) / model.mesh.radius

			local sel_slider_clipping_plane_point, new_clipping_plane_pos_factor = imgui.SliderFloat("Clipping plane point", plane_pos_factor, -1., 1.)
			if (sel_slider_clipping_plane_point) then 

				local new_clipping_plane_pos = center_at + new_clipping_plane_pos_factor * model.mesh.radius



				local v
				if (sel_clipping_plane[k] == 1) then
					v = vec3{new_clipping_plane_pos, clipping_plane_point.y, clipping_plane_point.z}
				elseif (sel_clipping_plane[k] == 2) then
					v = vec3{clipping_plane_point.x, new_clipping_plane_pos, clipping_plane_point.z}
				elseif (sel_clipping_plane[k] == 3) then
					v = vec3{clipping_plane_point.x, clipping_plane_point.y, new_clipping_plane_pos}
				end


				for _, material in pairs(model.materials) do 
					material["clipping"]["point"] = v
				end
			end

			local sel_invert_clipping, new_invert_clipping = imgui.Checkbox("Invert clipping", invert_clipping)
			if (sel_invert_clipping) then 
				invert_clipping = new_invert_clipping

				for _, material in pairs(model.materials) do 
					material["clipping"]["invert"] = invert_clipping
				end
			end

		end


		if (imgui.CollapsingHeader("Style##" .. k .. "_properties_style")) then 

			-- for material_name, material in pairs(model.materials) do
			-- 	imgui.SeparatorText("Material: " .. material_name)
			-- 	for params_name, params in pairs(material.params) do
			-- 		imgui.Text("params: " .. params_name)
			-- 	end
			-- end

			local mesh_material = model:get_material("mesh")

			if mesh_material then

				local mesh_style = mesh_material["style"]

				local sel_mesh_visible, new_mesh_visible = imgui.Checkbox("Show mesh", mesh_material.visible)
				if (sel_mesh_visible) then 
					print("Change mesh visibility: " .. tostring(new_mesh_visible))
					mesh_material.visible = new_mesh_visible
				end

				local sel_color, new_color = imgui.ColorEdit3("Color", mesh_style.color)
				if (sel_color) then 
					print("Change color: " .. tostring(new_color))
					mesh_style.color = new_color
				end

				local sel_slider_mesh_size, new_mesh_size = imgui.SliderFloat("Mesh size", mesh_style.size, 0, 20)
				if (sel_slider_mesh_size) then 
					print("Change mesh size: " .. tostring(new_mesh_size))
					mesh_style.size = new_mesh_size
				end

				local sel_slider_mesh_shrink, new_mesh_shrink = imgui.SliderFloat("Mesh shrink", mesh_style.shrink, 0, 1)
				if (sel_slider_mesh_shrink) then 
					print("Change mesh shrink: " .. tostring(new_mesh_shrink))
					mesh_style.shrink = new_mesh_shrink
				end

				local sel_corner_visible, new_corner_visible = imgui.Checkbox("Show corners", mesh_style.corner_visible)
				if (sel_corner_visible) then 
					print("Change corner visibility: " .. tostring(new_corner_visible))
					mesh_style.corner_visible = new_corner_visible
				end

			end

			local points_material = model:get_material("points")

			if points_material then
				local points_style = points_material["style"]

				local sel_point_visible, new_point_visible = imgui.Checkbox("Show points", points_material.visible)
				if (sel_point_visible) then 
					print("Change point visibility: " .. tostring(new_point_visible))
					points_material.visible = new_point_visible
				end

				local sel_point_color, new_point_color = imgui.ColorEdit3("Point color", points_style.color)
				if (sel_point_color) then 
					-- print("Change point color: " .. tostring(new_point_color))
					points_style.color = new_point_color
				end

				local sel_point_size, new_point_size = imgui.SliderFloat("Point size", points_style.size, 0, 50)
				if (sel_point_size) then 
					print("Change point size: " .. tostring(new_point_size))
					points_style.size = new_point_size
				end
			end

			local halfedges_material = model:get_material("halfedges")
			if halfedges_material then 

				local haldedges_style = halfedges_material["style"]

				local sel_edge_visible, new_edge_visible = imgui.Checkbox("Show edges", halfedges_material.visible)
				if (sel_edge_visible) then 
					print("Change edge visibility: " .. tostring(new_edge_visible))
					halfedges_material.visible = new_edge_visible
				end

				local sel_edge_thickness, new_edge_thickness = imgui.SliderFloat("Edge thickness", haldedges_style.thickness, 0, 50)
				if (sel_edge_thickness) then 
					print("Change edge thickness: " .. tostring(new_edge_thickness))
					haldedges_style.thickness = new_edge_thickness
				end

				local sel_edge_spacing, new_edge_spacing = imgui.SliderFloat("Edge spacing", haldedges_style.spacing, 0, 1)
				if (sel_edge_spacing) then 
					print("Change edge spacing: " .. tostring(new_edge_spacing))
					haldedges_style.spacing = new_edge_spacing
				end

				local sel_edge_padding, new_edge_padding = imgui.SliderFloat("Edge padding", haldedges_style.padding, 0, 1)
				if (sel_edge_padding) then 
					print("Change edge padding: " .. tostring(new_edge_padding))
					haldedges_style.padding = new_edge_padding
				end

				local sel_edge_inside_color, new_edge_inside_color = imgui.ColorEdit3("Edge inside color", haldedges_style.inside_color)
				if (sel_edge_inside_color) then 
					print("Change edge inside color: " .. tostring(new_edge_inside_color))
					haldedges_style.inside_color = new_edge_inside_color
				end

				local sel_edge_outside_color, new_edge_outside_color = imgui.ColorEdit3("Edge outside color", haldedges_style.outside_color)
				if (sel_edge_outside_color) then 
					print("Change edge outside color: " .. tostring(new_edge_outside_color))
					haldedges_style.outside_color = new_edge_outside_color
				end

			end


		end

		if (imgui.CollapsingHeader("Attributes##" .. k .. "_properties_attributes")) then 

			imgui.Text("Attribute")

			-- Get current model attributes
			local attributes = model.mesh.attributes

			-- Get selected colormap for current model
			local selected_colormap = "CET-R41"
			if selected_colormaps[model.name] ~= nil then 
				selected_colormap = selected_colormaps[model.name]
			end

			if (#attributes > 0) then

				local selected_attr = model.selected_attribute
				
				local selected_attr_label = "None"
				if selected_attr then 
					selected_attr_label =  selected_attr.fullname
				end

				if (imgui.BeginCombo("##combo_attribute0_selection", selected_attr_label)) then

					local is_selected = selected_attr == nil
					if (imgui.Selectable("None", is_selected)) then
						model.selected_attribute = nil
					end

					for i, attr in ipairs(attributes) do
						local cur_attr_label = attr.fullname
						
						local is_selected = attr == selected_attr

						if (imgui.Selectable(cur_attr_label, is_selected)) then
							-- Set attribute & colormap
							model:set_colormap(app.scene.colormaps[selected_colormap])
							model.selected_attribute = attr
						end
					end
					imgui.EndCombo()
				end

				-- Display attribute range
				if selected_attr then 
					local l = model.layers[Layer.COLORMAP_0][selected_attr.kind]
					local sel_range, new_range = imgui.InputFloat2("Range", l.range)
					if sel_range then 
						-- print(new_range:to_string())
						l.range = new_range
					end
					imgui.SameLine()
					if imgui.SmallButton("Autorange") then 
						l.range = selected_attr.range
					end
				end



			end

			imgui.Text("Colormap")

			-- local colormaps = app.scene.colormaps
			-- local items = {}
			-- for i = 1, #colormaps do 
			-- 	table.insert(items, colormaps[i].name)
			-- end

			local colormap_size = imgui.ImVec2(320, 20)


			if (imgui.BeginCombo("##combo_colormaps0_selection", selected_colormap)) then
				-- Display items in the popup
				for colormap_name, colormap in pairs(app.scene.colormaps) do
					local is_selected = selected_colormap == colormap_name
					-- Create a unique ID for each item to prevent conflicts
					-- imgui.PushID(i)

					-- Calculate total width including spacing
					-- local total_width = imgui.CalcTextSize(items[i]).x + colormap_size.x + 10.0

					-- Display the item with both text and image
					if (imgui.Selectable(colormap_name .. "##selectable_colormap_" .. colormap_name, is_selected)) then
						selected_colormaps[model.name] = colormap_name
						model:set_colormap(colormap)
					end

					-- Display the image after the text
					imgui.Image(colormap.tex, colormap_size)

					-- imgui.PopID()
				end

				imgui.EndCombo()
			end

			local selected_cm = app.scene.colormaps[selected_colormap]
			if selected_cm.height > 1 then 
				local h = selected_cm.height / selected_cm.width * 320 * 0.5
				colormap_size = imgui.ImVec2(320, h)
			end

			imgui.Image(
				selected_cm.tex, 
				colormap_size
			)
		end

		imgui.Separator()

		if imgui.Button("Apply to all##Apply_all") then 
			apply_to_all(model, false)
		end
		imgui.SameLine()
		if imgui.Button("Apply to visibles##Apply_visibles") then 
			apply_to_all(model, true)
		end
end

function apply_to_all(model, only_visible)
	for cur_model_name, cur_model in pairs(app.scene.models) do
		if not(only_visible) or only_visible and cur_model.visible then
			cur_model:apply_material_from(model)
			-- for mat_name, mat in pairs(model.materials) do 
			-- 	local cur_mat = cur_model:get_material(mat_name)
			-- 	if cur_mat then 
			-- 		cur_mat:set(mat)
			-- 	end
			-- end
		end
	end
end

function layout_gui() 
	return {["Scene"] = "nav_bar"}
end

function draw_gui()
	imgui.Begin("Scene")

	if (imgui.BeginTabBar("Scene Tabs")) then 

		if (imgui.BeginTabItem("Flat view")) then

			local all_visible = true
			for k, model in pairs(app.scene.models) do
				if not model.visible then 
					all_visible = false 
					break 
				end
			end

			local sel_all_visible, new_all_visible = imgui.Checkbox("All visibles##all_visibility", all_visible)
			if (sel_all_visible) then 
				for k, model in pairs(app.scene.models) do
					model.visible = new_all_visible
				end
			end

			for _, model in ipairs(app.scene.models) do



				local sel_visible, new_visible = imgui.Checkbox("##" .. model.name, model.visible)
				if (sel_visible) then 
					model.visible = new_visible
				end


				imgui.SameLine()
				if app.scene.selected_model == model.name then 
					imgui.TextColored(1, 1, 1, 1, model.name)
				else 
					imgui.TextColored(0.5, 0.5, 0.6, 1, model.name)
				end

				if imgui.IsItemClicked(0) then
					app.scene.selected_model = model.name

					if imgui.IsMouseDoubleClicked(0) then 
						for model_name, m in pairs(app.scene.models) do
							m.visible = (m.name == model.name)
						end
					end
				end

				imgui.SameLine()
				if (imgui.Button("View##" .. "btn_view_" .. model.name)) then
					app.scene.current_camera:look_at_box(model.mesh.bbox)
				end
				
			end

			imgui.Separator()

			-- Check if app has at least one model, to draw properties of current one (if exists)
			if (app.scene.current_model) then
				draw_model_properties(app.scene.current_model, app.scene.selected_model, 0)
			end

			imgui.EndTabItem()

		end 

		if (imgui.BeginTabItem("Cameras")) then

			imgui.Text("Cameras")
			imgui.Separator()

			if (imgui.BeginListBox("##list_box_cameras")) then
				for k, _ in pairs(app.scene.cameras) do

					local is_selected = k == app.scene.selected_camera

					-- Add unique id to prevent conflicts
					if (imgui.Selectable(k .. "##list_box_selectable_" .. k, is_selected)) then
						app.scene.selected_camera = k
					end
					
				end
				imgui.EndListBox()
			end
			
			imgui.Text("Properties")
			imgui.Separator()
			local camera = app.scene.current_camera
			local p = camera.position

			-- Compute zoom factor (varies between 0.001 for 0% - 1 for 100%)
			-- local fov = app.scene.camera.fov
			-- local zoom_factor = -(fov - 45.) / (45. - 0.25) * 100.
			local zoom_factor = app.scene.current_camera.zoom * 100.
			
			
			-- Convert -0 value to 0
			if (zoom_factor <= 0 and zoom_factor > -0.001) then zoom_factor = 0 end

			local str_zoom_factor = string.format("%.0f", zoom_factor)

			imgui.Text("Zoom: " .. str_zoom_factor .. "%")
			imgui.SameLine()
			if (imgui.SmallButton("Reset zoom")) then
				app.scene.current_camera:reset_zoom()
			end

			imgui.Text("Position: " .. p:to_string());
			imgui.Text("Look at: " .. camera.look_at:to_string());

			imgui.EndTabItem()
		end

		if (imgui.BeginTabItem("Render options")) then

			local sel_background_color, new_background_color = imgui.ColorEdit3("Background Color", app.scene.default_render_surface.background_color)
			if (sel_background_color) then 
				app.scene.default_render_surface.background_color = new_background_color
			end
			
			imgui.EndTabItem()
		end

		imgui.EndTabBar()
	end



	imgui.End()

end

function key_event(key, scancode, action, mods)
	-- up: key 265, scancode: 103 
	-- down: key: 264, scancode: 125
	print("key: "..tostring(key))

	local sel_model_name = app.scene.selected_model

	local sel_i = 1
	local indexed_models = {}
	if action == 0 and (key == 265 or key == 264) then 
		local i = 1

		for model_name, m in pairs(app.scene.models) do
			indexed_models[i] = m.name
			if m.name == app.scene.selected_model then 
				sel_i = i
			end
			i = i + 1
		end

	end



	if action == 0 and key == 265 then 
		app.scene.selected_model = indexed_models[sel_i - 1]
	elseif action == 0 and key == 264 then 
		-- print("select: " .. indexed_models[sel_i])
		app.scene.selected_model = indexed_models[sel_i + 1]
	elseif key == 257 then 
		print("enter")
		for model_name, m in pairs(app.scene.models) do
			m.visible = (m.name == model.name)
		end
	elseif key == 71 then 
		print("go")

		for model_name, m in pairs(app.scene.models) do
			if m.name == model.name then
				app.scene.current_camera:look_at_box(model.mesh.bbox)
			end
		end
	end
end

-- function format_label_attr(attr)
-- 	if not attr then 
-- 		return "None"
-- 	end
-- 	return attr.name 
-- 	.. " (" .. element_kind_to_string(attr.kind) .. ")" 
-- 	.. " (" .. element_type_to_string(attr.type) .. ")"
-- 	.. " (" .. tostring(attr.dim) .. ")"
-- end