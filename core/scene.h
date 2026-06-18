#pragma once

#include "data/colormap.h"
#include "input_states.h"

#include "render_surface.h"

#include "cameras/camera.h"
#include "cameras/trackball_camera.h"
#include "cameras/descent_camera.h"
#include "cameras/camera_collection.h"


#include "renderers/shader_base.h"

#include "scene_node.h"
#include "renderers/point_style_params.h" // TODO remove test
#include "model_loader.h"// TODO remove test


#include <map>

class IApp;

struct Scene {
	
	Scene(IApp &app) : app(app) {}


	void init();
	std::shared_ptr<SceneNode> loadModel2(const std::string filename, const std::string name = "");

	void render();

	void clean() {
		// TODO clean nodes

		// Clear textures
		for (int i = 0; i < colormaps.size(); ++i)
			glDeleteTextures(1, &colormaps[i].tex);
	}

	void clear() {
		// TODO clear nodes
		selectNode("");
		cameras.clear();
		setupCameras();
		clearColormaps();
	}







	bool selectNode(std::string name) {
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

	const std::string getSelectedNodeName() {
		return selectedNode;
	}

	void focus2(std::string nodeName);

	inline std::shared_ptr<SceneNode> getCurrentNode() {
		if (!selectedNode.empty())
			return _nodes.at(selectedNode);

		return nullptr;
	}

	std::shared_ptr<SceneNode> getHoveredNode();


	std::tuple<glm::vec3, glm::vec3> computeSceneBBox2();
	float computeSceneDiameter2();
	void computeFarPlane2();


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

	std::vector<Colormap>& getColormaps() {
		return colormaps;
	}

	Colormap getColormap(const std::string name);
	Colormap getColormap(int idx);

	void loadState(json &j, const std::string filename);
	void saveState(json &j, const std::string filename);

	RenderSurface& getDefaultRenderSurface() { return *renderSurfaces["default"]; }
	std::map<std::string, std::shared_ptr<RenderSurface>>& getRenderSurfaces() { return renderSurfaces; }


	bool hasNodes() const {
		return _nodes.size() > 0;
	}

	const std::map<std::string, std::shared_ptr<SceneNode>>& getNodes() const {
		return _nodes;
	}

	// template <typename T>
	// std::map<std::string, std::shared_ptr<T>> getNodes2() const {
	// 	static_assert(std::is_base_of_v<SceneNode, T>, 
	// 				"T must be derived from SceneNode");
		
	// 	std::map<std::string, std::shared_ptr<T>> result;
		
	// 	for (const auto& [name, node] : _nodes) {
	// 		if (auto casted = std::dynamic_pointer_cast<T>(node)) {
	// 			result[name] = casted;
	// 		}
	// 	}
		
	// 	return result;
	// }

	std::shared_ptr<SceneNode> getNodeByName(const std::string name) {
		return _nodes.contains(name) ? _nodes.at(name) : nullptr;
	}

	// const std::vector<std::shared_ptr<SceneNode>> getNodesByName(const std::string name) {
	// 	std::vector<std::shared_ptr<SceneNode>> results;
	// 	for (auto &[_, node] : _nodes) {
	// 		if (node->getName() == name)
	// 			results.push_back(node);
	// 	}
	// 	return results;
	// }

	std::shared_ptr<SceneNode> getNodeByIndex(int index) {
		for (auto &[_, node] : _nodes) {
			if (node->getIndex() == index)
				return node;
		}

		return nullptr;
	}

	
	const std::map<std::string, std::unique_ptr<ShaderBase>>& getShaders() const {
		return _shaders;
	}

	private:
	IApp &app;

	std::string selectedNode = "";

	std::string selectedCamera = "default";
	CameraCollection cameras;

	std::map<std::string, std::shared_ptr<SceneNode>> _nodes;

	std::map<std::string, std::unique_ptr<ShaderBase>> _shaders;


	// display color map in good format for 2D in the UI
	std::vector<Colormap> colormaps;

	std::map<std::string, std::shared_ptr<RenderSurface>> renderSurfaces;
	
};