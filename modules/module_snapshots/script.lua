local files
local screenshot_files
local snapshots = {}

function init()
	print("start list")
	
	files = list_directory("snapshots", ".json")
	screenshot_files = list_directory("snapshots", ".jpg")
	
	-- for i = 1, #files do
	-- 	local name = tostring(files[i])
	-- 	print("found file, " .. name)
	-- 	table.insert(snapshots, name)
	-- end
	-- print("end list")
end

function draw_gui() 
	imgui.Begin("Snapshots")

	for i = 1, #files do 
		imgui.Text(files[i])
		imgui.Image(0, imgui.ImVec2(320, 35))
	end
	-- for k, v in pairs(snapshots) do 
	-- 	imgui.Text(k)
	-- end



	imgui.End()
end

function key_event(key, scancode, action, mods)
	print("key:" .. tostring(key) .. ", scancode: " .. tostring(scancode))

end