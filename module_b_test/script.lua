function init()
	print("Hellow ! Script B loaded !")

	app.print_test()
	app.test = 1;
	app.print_test()
	app.test = 2;
	app.print_test()



	print(app.test)

	print("END INIT")
end

-- function key_event(key, scancode, action, mods)
-- 	print("Key event: " .. key .. ", " .. scancode .. ", " .. action .. ", " .. mods)
-- 	if (key == 78) then 
-- 		app.reset_zoom();
-- 	end
-- end
local b = false
function draw_gui()
	if (imgui.Begin("Script B")) then

		imgui.Text("Hello script B !")

		imgui.Button("ok0")

		
		local a, res = imgui.Checkbox("Enable light", b)
		b = a
		print("Is checked: " .. tostring(b))

		-- imgui.SliderFloat("slider", 0, 0, 10)
		-- imgui.InputText("text", "my text", 100)
		imgui.End()
	end

end