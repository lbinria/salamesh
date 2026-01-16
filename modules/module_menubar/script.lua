function draw_gui()
	if imgui.BeginMainMenuBar() then 
		if imgui.BeginMenu("File") then 

			if imgui.MenuItem("Open model") then 
				app:show_open_model_dialog()
			end

			if imgui.MenuItem("Save model as") then 
				app:show_save_model_dialog()
			end

			if imgui.MenuItem("Clear scene") then 
				app:clear_scene()
			end

			if imgui.MenuItem("Save state") then 
				app:snapshot()
			end

			if imgui.MenuItem("Load state") then 
				app:load_snapshot()
			end

			if imgui.MenuItem("Quit") then 
				app:quit()
			end

			imgui.EndMenu()
		end

		imgui.EndMainMenuBar()
	end
end