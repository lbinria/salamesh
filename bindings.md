# Application Bindings Documentation

## Type Bindings

| Property | Access | Type | Description |
|----------|--------|------|-------------|
| tex_id | Read-only | Integer | Texture identifier |
| width | Read-only | Integer | Image width |
| height | Read-only | Integer | Image height |
| channels | Read-only | Integer | Number of color channels |

| Property | Access | Type | Description |
|----------|--------|------|-------------|
| state_filename | Read-only | String | Filename of the saved state |
| thumb_filename | Read-only | String | Filename of the thumbnail |
| image | Read-only | Image | Associated image object |

| Property | Access | Type | Description |
|----------|--------|------|-------------|
| hovered | Read-write | Boolean | Current hover state |
| all_hovered | Read-only | Boolean | Whether all elements are hovered |
| any_hovered | Read-only | Boolean | Whether any element is hovered |
| has_changed | Read-only | Boolean | Whether state has changed |

## App Functions

### Scene Management

| Function | Parameters | Return Type | Description |
|----------|------------|-------------|-------------|
| clear_scene | - | void | Clear the current scene |
| quit | - | void | Close the application |

### Dialogs

| Function | Parameters | Return Type | Description |
|----------|------------|-------------|-------------|
| show_open_model_dialog | - | void | Open model file dialog |
| show_save_model_dialog | - | void | Save model file dialog |

### State Management

| Function | Parameters | Return Type | Description |
|----------|------------|-------------|-------------|
| load_state | - | void | Load application state |
| save_state | - | void | Save current application state |
| snapshot | - | void | Create a snapshot |
| load_snapshot | - | void | Load a previously saved snapshot |
| list_snapshots | - | List of Snapshots | Retrieve available snapshots |

### Model Management

| Function | Parameters | Return Type | Description |
|----------|------------|-------------|-------------|
| load_model | (string filename) | void | Load a model from file |
| add_model | (string name, int type) | int | Add a new model (returns 1-based index) |
| remove_model | (int idx OR string name) | void/bool | Remove a model by index or name |
| get_model_by_name | (string name) | Model | Retrieve model by name |
| get_index_of_model | (string name) | int | Get 1-based index of model |
