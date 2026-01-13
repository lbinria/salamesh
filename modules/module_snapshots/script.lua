local snapshots = {}

function init()
	print("list snapshots")
	snapshots = app:list_snapshots()
	print('ok')
end

function draw_gui() 
	imgui.Begin("Snapshots")

	for i = 1, #snapshots do 
		local snap = snapshots[i]
		local thumb = snap.image
		if imgui.ImageButton("load_" .. i, thumb.tex_id, imgui.ImVec2(thumb.width, thumb.height)) then 
			app:load_state(snap.state_filename)
		end 
	end

	imgui.End()
end

function key_event(key, scancode, action, mods)
	print("key:" .. tostring(key) .. ", scancode: " .. tostring(scancode) .. ", action:" .. action .. ", mod: " .. tostring(mods))
	-- On Ctrl+S save state
	if key == 83 and mods == 2 and action == 1 then 
		local snap = app:snapshot()
		table.insert(snapshots, 1, snap)
	end
end