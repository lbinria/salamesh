# SalaMesh

![](logo.jpeg)

A scientific mesh engine.
Forge your own meshing application in the fire of salamesh.

## Installation

### Windows

You may find pre-build binaries in releases.

### Linux (Debian / Ubuntu)

As a linux user, we know how much fun compiling is !

if you haven't already done so, install tools for building:

```
sudo apt-get update
sudo apt install -y build-essential cmake pkg-config git
```

Or just run the following:

```
# Clone repo
git clone https://github.com/lbinria/salamesh.git
```
```
# Install dev libs (OpenGL / GLFW / Lua)
sudo apt install -y libgl1-mesa-dev libglu1-mesa-dev libglfw3 libglfw3-dev lua5.4 liblua5.4-dev
```
```
# Go to the salamesh directory & configure cmake
cd salamesh && cmake -B build`
```
```
# Compile
cd build &&  cmake --build . --parallel
```

## Dependencies

All dependencies sources are directly included except lua for Linux users.

 - ImGui & ImGuiFileDialog-0.6.7 - User interface
 - glfw - Windows & Inputs management
 - glm - Linear algebra
 - sol2 - Lua bindings
 - stb_image - Loading / Writing images
 - lua-5.4.2 - Lua Interop
 - nlomann json - Json parser