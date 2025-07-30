function init()
	
end

function draw_gui()
	imgui.Begin("Attributes lua script")

	local cur_model = app.model

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
			local attr_name, attr_element = cur_model.attrs[1][1]
			local attr_element = cur_model.attrs[1][2]
			-- local attr_name, attr_element = cur_model.get_attr(1);
			-- print("first attr:" .. attr_name)
			-- print("second attr:" .. attr_element)
			-- print("selected atrt:" .. tostring(cur_model.selected_attr))")
			if (imgui.BeginCombo("##combo_attribute_selection", cur_model.attrs[cur_model.selected_attr + 1][1])) then
				for n = 1, #cur_model.attrs do
					local is_selected = (n - 1) == cur_model.selected_attr
					if (imgui.Selectable(cur_model.attrs[n][1], is_selected)) then
						cur_model.selected_attr = n - 1

						-- print("set attr: " .. cur_model.attrs[n][1] .. ":" .. cur_model.attrs[n][2] .. ":" .. cur_model.attrs[n][3])
					end
				end
				imgui.EndCombo()
			end
		end


	end 

	local sel_selected, new_selected = imgui.InputInt("Selected", app.selected_model)
	if (sel_selected) then 
		app.selected_model = new_selected
	end

	imgui.End()
end