#pragma once

#include "colormap.h"
#include "input_states.h"

#include "../render_surface.h"

#include "camera.h"
#include "trackball_camera.h"
#include "descent_camera.h"
#include "camera_collection.h"


#include "shader_base.h"

#include "scene_node.h"
#include "point_style_params.h" // TODO remove test
#include "../model_loader.h"// TODO remove test


#include <map>

class IApp;

struct Scene {
	
	Scene(IApp &app) : app(app) {}


	void init();
	std::shared_ptr<SceneNode> loadModel(const std::string filename, const std::string name = "");

	void render();


	void clean() {
		// TODO clean nodes

		// Clear textures
		for (auto &[_, colormap] : colormaps)
			glDeleteTextures(1, &colormap.tex);
	}

	void clear() {
		// TODO clear nodes
		setSelectedNode("");
		cameras.clear();
		setupCameras();
		clearColormaps();
	}

	bool setSelectedNode(std::string name) {
		if (name.empty())
			return false;

		if (!_nodes.contains(name)) {
			std::cerr << "Invalid model selection: " << name << std::endl;
			return false;
		}

		auto oldSelection = selectedNode;
		selectedNode = name;
		// TODO important reactivate this !
		// notifySelectedModelChanged(oldSelection, name);
		return true;
	}

	const std::string getSelectedNode() {
		return selectedNode;
	}

	void focus(std::string nodeName);

	inline std::shared_ptr<SceneNode> getCurrentNode() {
		if (!selectedNode.empty())
			return _nodes.at(selectedNode);

		return nullptr;
	}

	std::shared_ptr<SceneNode> getHoveredMesh();


	std::tuple<vec3, vec3> computeSceneBBox();
	float computeSceneDiameter();
	void updateFarPlane();


	void setupCameras();


	bool setSelectedCamera(std::string selected) {
		if (selected.empty())
			return false;

		if (!cameras.has(selected)) {
			std::cerr << "Invalid camera selection: " << selected << std::endl;
			return false;
		}

		// Set camera to render surface
		getDefaultRenderSurface().setCamera(cameras[selected]);
		selectedCamera = selected;
		return true;
	}
	
	std::string getSelectedCamera() {
		return selectedCamera;
	}

	Camera& getCurrentCamera() { return *cameras[selectedCamera]; }
	CameraCollection& getCameras() { return cameras; }


	void setupColormaps();

	void addColormap(const std::string name, const std::string filename);
	void removeColormap(const std::string name);

	void clearColormaps() {
		colormaps.clear();
		setupColormaps();
	}

	const std::map<std::string, Colormap>& getColormaps() {
		return colormaps;
	}

	Colormap getColormap(const std::string name);

	void loadState(json &j, const std::string filename);
	void saveState(json &j, const std::string filename);

	RenderSurface& getDefaultRenderSurface() { return *renderSurfaces["default"]; }
	std::map<std::string, std::shared_ptr<RenderSurface>>& getRenderSurfaces() { return renderSurfaces; }


	bool hasNodes() const {
		return _nodes.size() > 0;
	}

	int countNodes() const {
		return static_cast<int>(_nodes.size());
	}

	std::shared_ptr<SceneNode> createNode(const std::string name, std::shared_ptr<Geometry> geometry) {
		auto node = std::make_shared<SceneNode>(name, geometry);
		_nodes.emplace(node->getName(), node);
		return node;
	}

	const std::vector<std::shared_ptr<SceneNode>> getNodes() const {
		std::vector<std::shared_ptr<SceneNode>> result;
		for (auto &[_, node] : _nodes)
			result.push_back(node);
		
		return result;
	}

	std::shared_ptr<SceneNode> findNodeByName(const std::string name) {
		return _nodes.contains(name) ? _nodes.at(name) : nullptr;
	}

	std::shared_ptr<SceneNode> findNodeByIndex(int index) {
		for (auto &[_, node] : _nodes) {
			if (node->getIndex() == index)
				return node;
		}

		return nullptr;
	}

	const std::vector<std::unique_ptr<ShaderBase>>& getShaders() const {
		return _shaders;
	}

	std::optional<std::reference_wrapper<ShaderBase>> getShader(const std::string name) const {
		for (auto &shader : _shaders) {
			if (shader->getName() == name) {
				return *shader;
			}
		}
		return std::nullopt;
	}

	private:
	IApp &app;

	std::string selectedNode = "";

	std::string selectedCamera = "default";
	CameraCollection cameras;

	std::map<std::string, std::shared_ptr<SceneNode>> _nodes;
	// std::map<std::string, std::unique_ptr<ShaderBase>> _shaders;
	std::vector<std::unique_ptr<ShaderBase>> _shaders;

	// display color map in good format for 2D in the UI
	std::map<std::string, Colormap> colormaps;

	std::map<std::string, std::shared_ptr<RenderSurface>> renderSurfaces;
	
	void render(std::shared_ptr<SceneNode> node, std::unique_ptr<ShaderBase> &shader, std::map<std::string, bool> &wasUpdated);



};