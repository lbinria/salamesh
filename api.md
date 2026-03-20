# API

Caution: At the moment, this doc is partially generated and can not reflect the actual one.

### Script callback functions

Callback functions are functions called by the app. There is 3 categories of callback functions:

 - `Lifecycle`: called at some point of the execution of the app
 - `Events`: called when a specific event is triggered by the user / app
 - `Configuration`: specific function that does return data to specify how the app should use or format some elements

| Function | Description | Example |
|------------------------|-------------|---------|
| **Lifecycle functions** |||
| `init()` | Called when the script is initialized, generally at the initialization of app | [Details](signal_examples.md#init) |
| `draw_gui()` | Called when app is drawing the user interface | [Details](signal_examples.md#draw_gui) |
| `update(dt: float)` | Called when app update rendering (dt is the delta time in second) | [Details](signal_examples.md#update) |
| `cleanup()` | Called when the script is cleaned, generally when closing app | |
| **Input event functions** |||
| `mouse_move(x: float, y: float)` | Called on mouse move. (x, y) is the position of the mouse | |
| `mouse_button(button: int, action: int, mods: int)` | Called on mouse click. | |
| `mouse_scroll(xoffset: float, yoffset: float)` | Called on mouse scroll. (xoffset, yoffset) are delta values of scrolling on axes (x, y) | |
| `key_event(key: int, scan_code: int, action: int, mods: int)` | Called on key press | Details |
| **App event functions** |||
| `navigation_path_changed(old_nav_path: string list, new_nav_path: string list)` | Called immediately after navigation path has changed | Details |
| `model_loaded(name: string)` | Called immediately after a model was loaded | Details |
| `selected_model_changed(old_name: string, new_name: string)` | Called immediately after the model's selection change | Details |
| `scene_cleared()` | Called immediately after the scene was cleared | Details |
| **Config functions**|||
| `layout_gui()` | Enable to specify where the user interface must be draw | |

#### Examples



### App

There is only one instance of App, it is already declared and accessible using `app` object.
`app` is the root object of the core API.

| Function/Property Name | Type | Parameters | Description | Return Type |
|------------------------|------|------------|-------------|-------------|
| **App functions** |||
| `quit` | Function | — | Exits the application | `void` |
| `is_debug` | Read-only Property | — | Check whether application was compiled in Debug mode | `bool` |
| **Scene Management** |||
| `clear_scene` | Function | — | Clears the current scene | `void` |
| **Model Management** |||
| `load_model` | Function | `(filename: string, [name: string])` | Loads a model | `Model` | null` |
| `add_model` | Function | `(type: string, name: string)` | Adds a model with given type and name | `Model` |
| `remove_model` | Function | `(name: string)` | Removes a model by name | `void` |
| `get_model` | Function | `(name: string)` | Retrieves model by name | `Model` |
| `count_models` | Read-only Property | — | Returns number of models | `int` |
| `has_models` | Read-only Property | — | Checks if there is at least one model in scene | `bool` |
| `models` | Read-only Property | — | Returns list of models | `Model list` |
| `get_children_of` | Function | Likely model-related | Gets children of a model | `Model list` |
| `model` | Read-only Property | — | Gets current model | `Model` |
| `hovered_model` | Read-only Property | — | Gets hovered model | `Model` |
| `selected_model` | Property | `string` | Gets/sets selected model by name | `string` |
| `focus` | Function | `(name: string)` | Focus model | `void` |
| `has_model` | Function | `(name: string)` | Check if model of given name exists | `bool` |
| `get_model_name_by_index` | Function | `(i: int)` | Returns model's name of given index | `string` |
| `get_model_index_by_name` | Function | `(name: string)` | Returns model's index of given name | `int` |
| **Camera Management** |||
| `add_camera` | Function | `(type: string, name: string)` | Adds a model with given type and name | `Camera` |
| `remove_camera` | Function | `(name: string)` | Removes a camera by name | `void` |
| `cameras` | Read-only Property | — | Returns camera collection | `Camera collection` |
| `camera` | Read-only Property | — | Gets current camera | `Camera` |
| `get_camera` | Function | `(name: string)` | Retrieves camera by name | `Camera` |
| `count_cameras` | Read-only Property | — | Returns number of cameras | `int` |
| `has_cameras` | Read-only Property | — | Checks if there is at least one camera in scene | `bool` |
| `has_camera` | Function | `(name: string)` | Check if model of given name exists | `bool` |
| `clear_cameras` | Function | `()` | Remove all cameras from scene | `void` |
| `selected_camera` | Property | `(selected: string)` | Gets/sets selected camera | `string` |
| `input_state` | Read-only Property | — | Gets current input state | Input state type |
| **Renderer Management** |||
| `add_renderer` | Function | `(type: string, name: string)` | Adds a renderer with given type and name | `Renderer` |
| `remove_renderer` | Function | `(name: string)` | Removes a renderer by name | `void` |
| `renderers` | Read-only Property | — | Returns camera collection | `Renderer collection` |
| `get_renderer` | Function | `(name: string)` | Retrieves renderer by name | `Renderer` |
| `count_renderer` | Read-only Property | — | Returns number of renderer | `int` |
| `has_renderer` | Function | `(name: string)` | Check if renderer of given name exists | `bool` |
| `has_renderers` | Read-only Property | — | Checks if there is at least one renderer in scene | `bool` |
| `clear_renderers` | Function | `()` | Remove all renderers from scene | `void` |
| **Colormaps Management** |||
| `colormaps` | Read-only Property | — | Returns colormap collection | `Colormap collection` |
| `add_colormap` | Function | `(name: string, filename: string)` | Adds a colormap | `void` |
| `remove_colormap` | Function | `(name: string)` | Removes a colormap | `void` |
| `get_colormap` | Overloaded Function | `(idx: int)` or `(name: string)` | Gets a colormap | `Colormap` |
| **States** |||
| `load_state` | Function | `(filename: string)` | Loads a previously saved application state | `bool` |
| `save_state` | Function | `(filename: string)` | Saves the current application state | `bool` |
| `snapshot` | Function | — | Creates a snapshot of current state | `Snapshot` |
| `load_snapshot` | Function | — | Loads a previously created snapshot | `void` |
| `list_snapshots` | Function | — | Returns list of available snapshots | `List of Snapshot` |
| `screenshot` | Function | `(filename: string)` | Takes a screenshot | `void` |
| **Graphics** |||
| `setCullMode` | Function | `(mode: int)` | Sets culling mode | `void` |
| `cull_mode` | Write-only Property | `(mode: int)` | Sets culling mode | `void` |
| `cull` | Property | — | Gets/sets culling | Cull state |
| **Picking** |||
| `pick_point` | Function | `(x: double, y: double)` | Pick 3D point at (x,y) | `vec3` |
| `pick_edge` | Function | `(x: double, y: double)` | Pick edge id at (x,y) | `long` |
| `pick_mesh` | Function | `(x: double, y: double)` | Pick mesh id at (x,y) | `long` |
| `pick_cells` | Function | `(x: double, y: double, r: int)` | Pick cells ids at (x,y) in circle of radius r | `long list` |
| `pick_facets` | Function | `(x: double, y: double, r: int)` | Pick facets ids at (x,y) in circle of radius r | `long list` |
| `pick_vertices` | Function | `(x: double, y: double, r: int)` | Pick vertices ids at (x,y) in circle of radius r | `long list` |
| **Navigation** |||
| `navigation_path` | Property | `string \| string list` | Gets/sets navigation path | `NavigationPath` |
| `add_navigation_path` | Function | `(path_component: string)` | Adds a path component to navigation path | `void` |
| `top_navigation_path` | Function | — | Go to top navigation path | `void` |
| **Dialogs** |||
| `show_open_model_dialog` | Function | — | Displays dialog to open a model | `void` |
| `show_save_model_dialog` | Function | — | Displays dialog to save a model | `void` |

___

### `InputState`

| Property | Access | Type | Description |
|----------|--------|------|-------------|
| `vertex` | Read-write | `HoverState` | Get vertex hover state |
| `edge` | Read-only | `HoverState` | Get edge hover state |
| `facet` | Read-only | `HoverState` | Get facet hover state |
| `cell` | Read-only | `HoverState` | Get cell hover state |
| `mesh` | Read-only | `HoverState` | Get mesh hover state |
| `mouse` | Read-only | `MouseState` | Get mouse state |

___

### `HoverState`

| Property | Access | Type | Description |
|----------|--------|------|-------------|
| `hovered` | Read-only | `Number` | Get the primitive id that is currently hovered by mouse |
| `all_hovered` | Read-only | `Number list` | Get the primitive ids that are currently hovered by mouse |
| `any_hovered` | Read-only | `Boolean` | Indicates whether a primitive is currently hovered by mouse |
| `has_changed` | Read-only | `Boolean` | Indicates whether the primitive currently hovered by mouse has changed |

___

### `MouseState`

| Property | Access | Type | Description |
|----------|--------|------|-------------|
| `pos` | Read-only | `vec2` | Current mouse position |
| `last_pos` | Read-only | `vec2` | Previous mouse position |
| `delta` | Read-only | `vec2` | Change in position since last frame |
| `buttons` | Read-only | `Table` (8 booleans) | State of 8 mouse buttons (1-indexed) |
| `dbl_buttons` | Read-only | `Table` (8 booleans) | Double-click state of 8 mouse buttons (1-indexed) |
| `last_clicks` | Read-only | `Table` (8 numbers) | Timestamp of last click for each button (1-indexed) |
| `cursor_radius` | Read-only | `Number` | Radius of the cursor |
| `dbl_click_interval` | Read-only | `Number` | Time interval for detecting double-clicks |
| `is_left_button` | Read-only | `Boolean` | Indicated whether left mouse button is pressed |
| `is_right_button` | Read-only | `Boolean` | Indicated whether right mouse button is pressed |
| `is_middle_button` | Read-only | `Boolean` | Indicated whether middle mouse button is pressed |
| `is_button_pressed(button: Number)` | Function | `Boolean` | Indicated whether given button is currently pressed |
| `any_button_pressed` | Read-only | `Boolean` | Indicated whether any button is currently pressed |

___

### `NavigationPath`

| Function/Property | Type | Parameters | Return Type | Description |
|------|------|-----------|-------------|-------------|
| `get` | Function | — | `String list` | Return the navigation path as list of string |
| `str` | Function | — | `String` | Returns string representation of the path |
| `starts_with` | Function | `(head: String)` or `(head: Table[String])` | — | Check whether the path is prepend by a given head (string or table of strings) |
___

### `Snapshot`

A snapshot of the app state.

| Function/Property Name | Type | Parameters | Return Type | Description |
|------------------------|------|------------|-------------|-------------|
| `state_filename` | Read-only Property | — | `string` | Filename of the saved state |
| `thumb_filename` | Read-only Property | — | `string` | Filename of the thumbnail |
| `image` | Read-only Property | — | `Image` | Associated image object |

___

### `Image` 

Represent image / texture data.

| Function/Property Name | Type | Parameters | Return Type | Description |
|------------------------|------|------------|-------------|-------------|
| `tex_id` | Read-only Property | — | `int` | Texture identifier |
| `width` | Read-only Property | — | `int` | Image width |
| `height` | Read-only Property | — | `int` | Image height |
| `channels` | Read-only Property | — | `int` | Number of color channels |



## ImGui

| Function/Property Name | Type | Parameters | Description | Return Type |
|------------------------|------|------------|-------------|-------------|
| **Window Management** |||
| `Begin` | Function | `(name: string)` | Begins an ImGui window | `bool` |
| `End` | Function | — | Ends the current ImGui window | `void` |
| **Text Rendering** |||
| `Text` | Overloaded Function | `(text: string)` or `(format: string, ...args)` | Renders text | `void` |
| `TextColored` | Overloaded Function | `(r,g,b,a: float, text: string)` or `(r,g,b,a: float, format: string, ...args)` | Renders colored text | `void` |

| Function/Property Name | Type | Parameters | Description | Return Type |
|------------------------|------|------------|-------------|-------------|
| **Input Widgets** |||
| `InputText` | Function | `(label: string, initial_string: string, max_length: int)` | Text input field | `{changed: bool, new_text: string}` |
| `Button` | Function | `(label: string)` | Creates a button | `bool` |
| `ImageButton` | Function | `(label: string, texture_id, size: ImVec2)` | Creates an image button | `bool` |
| `Checkbox` | Function | `(label: string, initial_value: bool)` | Creates a checkbox | `{changed: bool, new_value: bool}` |
| `SliderFloat` | Function | `(label: string, initial_value: float, min: float, max: float)` | Float slider | `{changed: bool, new_value: float}` |
| `InputInt` | Function | `(label: string, initial_value: int)` | Integer input | `{changed: bool, new_value: int}` |
| `InputFloat` | Function | `(label: string, initial_value: float)` | Float input | `{changed: bool, new_value: float}` |
| `InputFloat2` | Function | `(label: string, initial_values: {float, float})` | 2D float input | `{changed: bool, new_values: [float, float]}` |
| `InputFloat3` | Function | `(label: string, initial_value: vec3)` | 3D float input | `{changed: bool, new_values: [float, float, float]}` |
| **Combo and Selection Widgets** |||
| `BeginCombo` | Function | `(label: string, preview_value: string)` | Starts a combo box | `bool` |
| `EndCombo` | Function | — | Ends a combo box | `void` |
| `BeginListBox` | Overloaded Function | `(label: string)` or `(label: string, size: ImVec2)` | Starts a list box | `bool` |
| `EndListBox` | Function | — | Ends a list box | `void` |
| `Selectable` | Function | `(label: string, selected: bool)` | Creates a selectable item | `bool` |