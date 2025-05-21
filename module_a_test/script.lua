function init()
	print("Hellow ! My script works !")
end

-- function key_event(key, scancode, action, mods)
-- 	print("Key event: " .. key .. ", " .. scancode .. ", " .. action .. ", " .. mods)
-- end

function draw_gui()
	imgui.Begin("My window's script")

	imgui.Text("Hello lua !")

	if (imgui.Button("Look at center")) then 
		app.look_at_center()
	end

	if (imgui.Button("Reset zoom")) then
		app.reset_zoom()
	end

	imgui.End()
end