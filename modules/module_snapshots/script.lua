-- local state_files
-- local screenshot_files
local snapshots = {}
-- local textures = {}

local snapshots_data = {}

function init()
	print("start list")
	
	-- state_files = list_directory("snapshots", ".json")
	-- screenshot_files = list_directory("snapshots", ".jpg")
	

	print("list snapshots")
	local snapshots = app:list_snapshots()
	for i = 1, #snapshots do 
		local snap = snapshots[i]
		print(snap[1])
		print(snap[2])

		local tex_id, w, h, c = load_image(snap[2])
		local snap_data = {tex_id, w, h, c, snap[1], snap[2]}
		print("texture: " .. tostring(tex_id))
		print("w: " .. tostring(w))
		print("h: " .. tostring(h))
		print("c: " .. tostring(c))
		table.insert(snapshots_data, snap_data)

		-- local state, image = snapshots[i]
		-- print("state: " .. state)
		-- print("image: " .. image)
	end

	-- for i = 1, #files do
	-- 	local name = tostring(files[i])
	-- 	print("found file, " .. name)
	-- 	table.insert(snapshots, name)
	-- end
	-- print("end list")
end

function draw_gui() 
	imgui.Begin("Snapshots")

	-- for i = 1, #state_files do 
	-- 	imgui.Text(state_files[i])
	-- 	-- imgui.Image(0, imgui.ImVec2(320, 35))
	-- 	imgui.Image(textures[i][1], imgui.ImVec2(textures[i][2], textures[i][3]))
	-- 	if imgui.Button("load_" .. i .. "##load") then
	-- 		app:load_state(state_files[i])
	-- 	end
	-- end

	for i = 1, #snapshots_data do 
		local data = snapshots_data[i]

		local tex = data[1]
		local w = data[2]
		local h = data[3]
		local c = data[4]
		local state_filename = data[5]
		local img_filename = data[6]

		-- imgui.Text(state_filename)
		
		-- imgui.Image(tex, imgui.ImVec2(w, h))
		-- if imgui.Button("load_" .. i .. "##load") then
		-- 	app:load_state(state_filename)
		-- end
		if imgui.ImageButton("load_" .. i, tex, imgui.ImVec2(w, h)) then 
			app:load_state(state_filename)
		end 
	end



	imgui.End()
end

function key_event(key, scancode, action, mods)
	print("key:" .. tostring(key) .. ", scancode: " .. tostring(scancode) .. ", mod: " .. tostring(mods))
	-- On Ctrl+S save state
	if key == 83 and mods == 2 then 
		app:save_state()
		-- Add to snapshots data
	end
end