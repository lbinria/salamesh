function init()
	print("Load scene module")
	print("Number of models: " .. tostring(#app.models))
end

function draw_tree(model)
	if (imgui.TreeNode(model.name)) then 
		local model_pos = model.world_position

		local sel_visible, new_visible = imgui.Checkbox("Visible##" .. model.name .. "_visible", model.visible)
		if (sel_visible) then 
			model.visible = new_visible
		end


		imgui.SameLine()
		if (imgui.SmallButton("View")) then 
			app.selected_model = i - 1
			-- Set camera position !
			app.camera.position = vec3.new(model_pos.x, model_pos.y, model_pos.z - 3.);
			app.camera.look_at = vec3.new(model_pos.x, model_pos.y, model_pos.z);
		end

		local p = model.position
		-- imgui.Text("world position: (%.4f, %.4f, %.4f)", model_pos.x, model_pos.y, model_pos.z);
		-- imgui.Text("local position: (%.4f, %.4f, %.4f)", p.x, p.y, p.z);
		-- TODO using ':' instead of '.' for to_string call... for sending self...
		imgui.Text("world position: " .. model_pos:to_string());
		imgui.Text("local position: " .. p:to_string());

		local children = app:getChildrenOf(model)

		for i = 1, #children do
			local child = children[i]
			draw_tree(child)
		end

		imgui.TreePop()
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
					app.selected_model = i - 1
					-- Set camera position !
					local model_pos = model.world_position
					app.camera.position = vec3.new(model_pos.x, model_pos.y, model_pos.z - 3.);
					app.camera.look_at = vec3.new(model_pos.x, model_pos.y, model_pos.z);
				end
				imgui.PopID()
			end

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

					draw_tree(model)
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