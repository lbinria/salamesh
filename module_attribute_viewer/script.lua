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
					-- app.renderer.selected_attr = app.renderer.selected_attr
					-- CellAttribute<double> a(attrs[selectedAttr].second, attributes, hex, -1);
					-- hex_renderer->changeAttribute(a, attrs[selectedAttr].first);
					-- app.renderer.changeAttribute(app.selected_attr)
				end

			end
		end
		imgui.EndCombo()
	end

	imgui.End()
end