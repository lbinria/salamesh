function init()
	
end

function draw_gui()
	imgui.Begin("Attributes lua script")

	if (imgui.BeginCombo("##Color Mode", app.renderer.color_mode_strings[app.renderer.color_mode + 1])) then
		for i = 1, #app.renderer.color_mode_strings do
			local is_selected = (i - 1) == app.pick_mode
			if (imgui.Selectable(app.renderer.color_mode_strings[i], is_selected)) then
				print("Change color mode to: " .. app.renderer.color_mode_strings[i])
				app.renderer.color_mode = i - 1

				if (app.renderer.color_mode == 1) then
					print("Change to attribute color mode")
					app.renderer.selected_attr = app.renderer.selected_attr
				end

			end
		end
		imgui.EndCombo()
	end

	if (app.renderer.color_mode == 1) then
		local items = {"Item1", "Item2"}
		local colormap_size = imgui.ImVec2(320, 35)

		if (imgui.BeginCombo("##combo_colormaps_selection", items[app.renderer.selected_colormap + 1])) then
			-- Display items in the popup
			for i = 1, #items do
				local is_selected = (app.renderer.selected_colormap + 1) == i
				-- Create a unique ID for each item to prevent conflicts
				imgui.PushID(i)

				-- Calculate total width including spacing
				-- local total_width = imgui.CalcTextSize(items[i]).x + colormap_size.x + 10.0

				-- Display the item with both text and image
				if (imgui.Selectable(items[i], is_selected)) then
					app.renderer.selected_colormap = i - 1
				end

				-- Display the image after the text
				imgui.Image(app.colormaps_2d[i], colormap_size)

				imgui.PopID()
			end

			imgui.EndCombo()
		end

		imgui.Image(
			app.colormaps_2d[app.renderer.selected_colormap + 1], 
			colormap_size
		)

		imgui.Text("Attribute")

		if (#app.renderer.attrs > 0) then
			-- local attr_name, attr_element = app.renderer.attrs[1]
			local attr_name, attr_element = app.renderer.attrs[1][1]
			local attr_element = app.renderer.attrs[1][2]
			-- local attr_name, attr_element = app.renderer.get_attr(1);
			-- print("first attr:" .. attr_name)
			-- print("second attr:" .. attr_element)
			-- print("selected atrt:" .. tostring(app.renderer.selected_attr))")
			if (imgui.BeginCombo("##combo_attribute_selection", app.renderer.attrs[app.renderer.selected_attr + 1][1])) then
				for n = 1, #app.renderer.attrs do
					local is_selected = (n - 1) == app.renderer.selected_attr
					if (imgui.Selectable(app.renderer.attrs[n][1], is_selected)) then
						app.renderer.selected_attr = n - 1

						-- print("set attr: " .. app.renderer.attrs[n][1] .. ":" .. app.renderer.attrs[n][2] .. ":" .. app.renderer.attrs[n][3])
					end
				end
				imgui.EndCombo()
			end
		end


	end 

	imgui.End()
end