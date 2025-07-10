function init()
	print("Load scene module")
	print("Number of models: " .. tostring(#app.models))
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
			if (imgui.TreeNode("Scene")) then 

				-- for k, model in app.models do 
				for i = 1, #app.models do
					-- TODO ImGuiTreeNodeFlags_Selected if model selected
					local model = app.models[i]
					if (model.parent == nil) then 
						goto continue
					end

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
						imgui.Text("world position: (%.4f, %.4f, %.4f)", model_pos.x, model_pos.y, model_pos.z);
						imgui.Text("local position: (%.4f, %.4f, %.4f)", p.x, p.y, p.z);

						imgui.TreePop()
					end
					::continue::
				end

				imgui.TreePop()
			end

			imgui.EndTabItem()
		end

		imgui.EndTabBar()
	end

	imgui.End()
end