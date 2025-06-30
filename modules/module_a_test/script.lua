function init()
	print("Hellow ! My script works !")
end

-- function key_event(key, scancode, action, mods)
-- 	print("Key event: " .. key .. ", " .. scancode .. ", " .. action .. ", " .. mods)
-- end

local mx = 0
local my = 0
function mouse_move(x, y)
	mx = x 
	my = y
end

-- function mouse_button(button, action, mods)
-- 	print("mouse button: " .. button .. "," .. action .. "," .. mods)
-- end

-- function mouse_scroll(xoffset, yoffset)
-- 	print("Scroll: " .. xoffset .. "," .. yoffset)
-- end

function draw_gui()
	imgui.Begin("My window's script")

	imgui.Text("Hello lua !")
	imgui.Text("Mouse pos: (" .. tostring(mx) .. "," .. tostring(my) .. ")")

	if (imgui.Button("Screenshot !")) then
		app.screenshot("screenshot_" .. os.date("%Y%m%d_%H%M%S") .. ".png")
	end

	imgui.End()
end