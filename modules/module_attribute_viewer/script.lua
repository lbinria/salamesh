function init()
	
end

function draw_gui()
	imgui.Begin("Attributes lua script")

	local cur_renderer = app.current_renderer

	if (imgui.BeginCombo("##Color Mode", cur_renderer.color_mode_strings[cur_renderer.color_mode + 1])) then
		for i = 1, #cur_renderer.color_mode_strings do
			local is_selected = (i - 1) == app.pick_mode
			if (imgui.Selectable(cur_renderer.color_mode_strings[i], is_selected)) then
				print("Change color mode to: " .. cur_renderer.color_mode_strings[i])
				cur_renderer.color_mode = i - 1

				if (cur_renderer.color_mode == 1) then
					print("Change to attribute color mode")
					cur_renderer.selected_attr = cur_renderer.selected_attr
				end

			end
		end
		imgui.EndCombo()
	end

	if (cur_renderer.color_mode == 1) then
		local items = {"Item1", "Item2"}
		local colormap_size = imgui.ImVec2(320, 35)

		if (imgui.BeginCombo("##combo_colormaps_selection", items[cur_renderer.selected_colormap + 1])) then
			-- Display items in the popup
			for i = 1, #items do
				local is_selected = (cur_renderer.selected_colormap + 1) == i
				-- Create a unique ID for each item to prevent conflicts
				imgui.PushID(i)

				-- Calculate total width including spacing
				-- local total_width = imgui.CalcTextSize(items[i]).x + colormap_size.x + 10.0

				-- Display the item with both text and image
				if (imgui.Selectable(items[i], is_selected)) then
					cur_renderer.selected_colormap = i - 1
				end

				-- Display the image after the text
				imgui.Image(app.colormaps_2d[i], colormap_size)

				imgui.PopID()
			end

			imgui.EndCombo()
		end

		imgui.Image(
			app.colormaps_2d[cur_renderer.selected_colormap + 1], 
			colormap_size
		)

		imgui.Text("Attribute")

		if (#cur_renderer.attrs > 0) then
			-- local attr_name, attr_element = cur_renderer.attrs[1]
			local attr_name, attr_element = cur_renderer.attrs[1][1]
			local attr_element = cur_renderer.attrs[1][2]
			-- local attr_name, attr_element = cur_renderer.get_attr(1);
			-- print("first attr:" .. attr_name)
			-- print("second attr:" .. attr_element)
			-- print("selected atrt:" .. tostring(cur_renderer.selected_attr))")
			if (imgui.BeginCombo("##combo_attribute_selection", cur_renderer.attrs[cur_renderer.selected_attr + 1][1])) then
				for n = 1, #cur_renderer.attrs do
					local is_selected = (n - 1) == cur_renderer.selected_attr
					if (imgui.Selectable(cur_renderer.attrs[n][1], is_selected)) then
						cur_renderer.selected_attr = n - 1

						-- print("set attr: " .. cur_renderer.attrs[n][1] .. ":" .. cur_renderer.attrs[n][2] .. ":" .. cur_renderer.attrs[n][3])
					end
				end
				imgui.EndCombo()
			end
		end


	end 

	local sel_selected, new_selected = imgui.InputInt("Selected", app.selected_renderer)
	if (sel_selected) then 
		app.selected_renderer = new_selected - 1
	end

	imgui.End()
end