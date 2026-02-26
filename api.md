# API

Caution: At the moment, this doc is totally generated and can not reflect the actual one.

### App

There is only one instance of App, it is already declared and accessible using `app` object.
`app` is the root object of the core API.

| Function/Property Name | Type | Parameters | Description | Return Type |
|------------------------|------|------------|-------------|-------------|
| **App functions** |||
| `quit` | Function | None | Exits the application | `void` |
| **Scene Management** |||
| `clear_scene` | Function | None | Clears the current scene | `void` |
| **Model Management** |||
| `load_model` | Function | `(filename: string)` | Loads a model | `bool` |
| `add_model` | Function | `(name: string, type: int)` | Adds a model with given name and type | `int` (index + 1) |
| `remove_model` | Overloaded Function | `(idx: int)` or `(name: string)` | Removes a model by index or name | `void` or `bool` |
| `get_model` | Function | `(name: string)` | Retrieves model by name | Model type |
| `get_index_of_model` | Function | `(name: string)` | Gets model index (1-based) | `int` |
| `count_models` | Read-only Property | None | Returns number of models | `int` |
| `has_models` | Read-only Property | None | Checks if models exist | `bool` |
| `models` | Read-only Property | None | Returns list of models | Model collection |
| `getChildrenOf` | Function | Likely model-related | Gets children of a model | Model collection |
| `model` | Read-only Property | None | Gets current model | Current model |
| `selected_model` | Property | `(selected: int)` | Gets/sets selected model (1-based) | `int` |
| **Camera Management** |||
| `cameras` | Read-only Property | None | Returns camera collection | Camera collection |
| `camera` | Read-only Property | None | Gets current camera | Current camera |
| `selected_camera` | Property | `(selected: int)` | Gets/sets selected camera (1-based) | `int` |
| `input_state` | Read-only Property | None | Gets current input state | Input state type |
| **Colormaps Management** |||
| `colormaps` | Read-only Property | None | Returns colormap collection | Colormap collection |
| `add_colormap` | Function | Colormap parameters | Adds a colormap | `void` |
| `remove_colormap` | Function | Colormap identifier | Removes a colormap | `void` |
| `get_colormap` | Overloaded Function | `(idx: int)` or `(name: string)` | Gets a colormap | Colormap |
| **States** |||
| `load_state` | Function | `(filename: string)` | Loads a previously saved application state | `bool` |
| `save_state` | Function | `(filename: string)` | Saves the current application state | `bool` |
| `snapshot` | Function | None | Creates a snapshot of current state | `Snapshot` |
| `load_snapshot` | Function | None | Loads a previously created snapshot | `void` |
| `list_snapshots` | Function | None | Returns list of available snapshots | `List of Snapshot` |
| `screenshot` | Function | `(filename: string)` | Takes a screenshot | `void` |
| **Graphics** |||
| `setCullMode` | Function | `(mode: int)` | Sets culling mode | `void` |
| `cull_mode` | Write-only Property | `(mode: int)` | Sets culling mode | `void` |
| `cull` | Property | None | Gets/sets culling | Cull state |
| **Navigation** |||
| `navigation_path` | Property | Path value | Gets/sets navigation path | Navigation path |
| `navigation_path_string` | Read-only Property | None | Gets navigation path as string | `string` |
| `add_navigation_path` | Function | Path parameters | Adds a navigation path | `void` |
| **Dialogs** |||
| `show_open_model_dialog` | Function | None | Displays dialog to open a model | `void` |
| `show_save_model_dialog` | Function | None | Displays dialog to save a model | `void` |

### Image 

Represent image / texture data.

| Function/Property Name | Type | Parameters | Return Type | Description |
|------------------------|------|------------|-------------|-------------|
| tex_id | Read-only Property | None | `int` | Texture identifier |
| width | Read-only Property | None | `int` | Image width |
| height | Read-only Property | None | `int` | Image height |
| channels | Read-only Property | None | `int` | Number of color channels |

### Snapshot

A snapshot of the app state.

| Function/Property Name | Type | Parameters | Return Type | Description |
|------------------------|------|------------|-------------|-------------|
| state_filename | Read-only Property | None | `string` | Filename of the saved state |
| thumb_filename | Read-only Property | None | `string` | Filename of the thumbnail |
| image | Read-only Property | None | `Image` | Associated image object |

### PrimitiveState

| Property | Access | Type | Description |
|----------|--------|------|-------------|
| hovered | Read-write | Boolean | Current hover state |
| all_hovered | Read-only | Boolean | Whether all elements are hovered |
| any_hovered | Read-only | Boolean | Whether any element is hovered |
| has_changed | Read-only | Boolean | Whether state has changed |

## ImGui

| Function/Property Name | Type | Parameters | Description | Return Type |
|------------------------|------|------------|-------------|-------------|
| **Window Management** |||
| `Begin` | Function | `(name: string)` | Begins an ImGui window | `bool` |
| `End` | Function | None | Ends the current ImGui window | `void` |
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
| `EndCombo` | Function | None | Ends a combo box | `void` |
| `BeginListBox` | Overloaded Function | `(label: string)` or `(label: string, size: ImVec2)` | Starts a list box | `bool` |
| `EndListBox` | Function | None | Ends a list box | `void` |
| `Selectable` | Function | `(label: string, selected: bool)` | Creates a selectable item | `bool` |