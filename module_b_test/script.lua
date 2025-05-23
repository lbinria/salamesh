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

local val = false
function draw_gui()
	imgui.Begin("Script B")

	imgui.Text("Hello script B !")

	
	local sel, newval = imgui.Checkbox("Enable light", val)
	if (sel) then 
		val = newval
		-- app.setLight(val);
	end

	-- imgui.SliderFloat("plop", 5, 0, 10)

	
	-- print("Is checked: " .. tostring(b))

	-- print("hel" .. true)

	-- imgui.SliderFloat("slider", 0, 0, 10)
	-- imgui.InputText("text", "my text", 100)
	
	imgui.End()

end