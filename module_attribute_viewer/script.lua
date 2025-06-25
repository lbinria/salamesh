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
		imgui.Text("Colormap")
		if (imgui.BeginCombo("##combo_colormaps_selection", items[app.renderer.selected_colormap + 1])) then
			-- Display items in the popup
			for i = 1, #items do
				local is_selected = (app.renderer.selected_colormap + 1) == i
				imgui.PushID(i)

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

	end 

	imgui.End()
end


-- if (hex_renderer->getColorMode() == 1) {


-- 	const char * items[] = {"Item1", "Item2"};

-- 	ImVec2 colormapSize(320, 35);
-- 	if (ImGui::BeginCombo("##combo_colormaps_selection", items[selectedColormap])) {

-- 		// Display items in the popup
-- 		for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
-- 			const bool isSelected = (selectedColormap == i);
			
-- 			// Create a unique ID for each item to prevent conflicts
-- 			ImGui::PushID(i);
			
-- 			// Calculate total width including spacing
-- 			float totalWidth = ImGui::CalcTextSize(items[i]).x + 320.f /* image width */ + 10.0f;
			
-- 			// Display the item with both text and image
-- 			if (ImGui::Selectable(items[i], isSelected)) {
-- 				selectedColormap = i;
-- 			}
			
-- 			// Display the image after the text
-- 			ImGui::Image(colormaps2D[i], colormapSize);
			
-- 			// Clean up the ID
-- 			ImGui::PopID();
-- 		}

		
		
-- 		ImGui::EndCombo();
-- 	}

-- 	ImGui::Image(
-- 		(ImTextureID)(intptr_t)colormaps2D[selectedColormap], 
-- 		colormapSize
-- 	);


-- 	ImGui::Text("Attribute");


-- 	if (hex_renderer->attrs.size() > 0) {
-- 		if (ImGui::BeginCombo("##combo_attribute_selection", std::get<0>(hex_renderer->attrs[hex_renderer->getSelectedAttr()]).c_str())) {
			
-- 			for (int n = 0; n < hex_renderer->attrs.size(); n++) {
-- 				bool isSelected = n == hex_renderer->getSelectedAttr();
-- 				if (ImGui::Selectable(std::get<0>(hex_renderer->attrs[n]).c_str(), isSelected)) {
-- 					hex_renderer->setSelectedAttr(n);
					
-- 					std::cout 
-- 						<< "set attr" 
-- 						<< std::get<0>(hex_renderer->attrs[n]) 
-- 						<< ":" << std::get<1>(hex_renderer->attrs[n]) 
-- 						<< ":" << std::get<2>(hex_renderer->attrs[n]) 
-- 					<< std::endl;
					
-- 				}
-- 			}
-- 			ImGui::EndCombo();
-- 		}
-- 	}


-- }