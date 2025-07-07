function init()
	print("Load scene module")
	print("Number of models: " .. tostring(#app.models))
end

function draw_gui()
	imgui.Begin("Scene")

	for i = 1, #app.models do
		local model = app.models[i]
		if (imgui.Button("Model " .. i .. ": " .. model.name)) then
			app.setCurrentModel(model)
		end
	end

	imgui.End()
end