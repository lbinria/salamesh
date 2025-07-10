function init()
	print("Load scene module")
	print("Number of models: " .. tostring(#app.models))
end

function draw_gui()
	imgui.Begin("Scene")

	for i = 1, #app.models do
		local model = app.models[i]
		imgui.PushID(i)
		if (imgui.Button("Model " .. i .. ": " .. model.name)) then
			app.selected_model = i - 1
			-- Set camera position !
			local model_pos = model.world_position
			app.camera.position = vec3.new(model_pos.x, model_pos.y, model_pos.z - 3.);
			app.camera.look_at = vec3.new(model_pos.x, model_pos.y, model_pos.z);
		end
		imgui.PopID()
	end

	imgui.End()
end