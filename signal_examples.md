# Callback examples

## A minimal lua script

!!!note
	Note that we shouldn't declare an empty callback function as they will be called by the app even if they are empty.

```lua
function init()
	print("Hello")
end

function draw_gui()
	-- Create a window with title My window
	imgui.Begin("My window")

	-- Create a button
	if imgui.Button("Click !") then 
		print("click...")
	end

	imgui.End()
end

function update(dt)
	-- Update something or delete this callback
end


```

## Callback functions details & examples

___

##### `init()`

```lua
function init()
	print("Hello there !")
end
```
___

##### `draw_gui()`

```lua
function draw_gui()
	-- Create a window with title My window
	imgui.Begin("My window")

	-- Create a button
	if imgui.Button("Click !") then 

	end

	imgui.End()
end
```

___

##### `update(dt: float)`

 - dt: Delta time in seconds

```lua
function update(dt)
	-- Get or create a line renderer
	local renderer = app:add_renderer("LineRenderer", "debug_line_renderer")

	if not renderer then 
		return
	end

	-- Cast Renderer to LineRenderer
	local lr = renderer:as("LineRenderer")
	lr:clear_lines()
	lr:add_line(Line{a = vec3{0., 0., 0.}, b = vec3{1., 0., 0.}, color = vec3{1., 0., 0.}})
	-- Push modifications
	lr:push()
end
```

___

##### `cleanup()`

```lua
function cleanup()
	print("clean up !")
end
```