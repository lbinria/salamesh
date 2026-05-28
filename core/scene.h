#pragma once

#include "data/colormap.h"
#include "input_states.h"

#include "render_surface.h"

#include "cameras/camera.h"
#include "cameras/trackball_camera.h"
#include "cameras/descent_camera.h"
#include "cameras/camera_collection.h"

#include "models/model.h"
#include "models/surface_model.h"
#include "models/volume_model.h"
#include "models/polyline_model.h"
#include "models/model_collection.h"

#include "renderers/renderer.h"
#include "renderers/line_renderer.h"
#include "renderers/renderer_collection.h"

#include "scene_node.h"
#include "renderers/point_style_params.h" // TODO remove test
#include "model_loader.h"// TODO remove test

#include <map>

class IApp;

struct Scene {
	
	Scene(IApp &app) : app(app) {}


	void init();
	std::shared_ptr<SceneNode> loadModel2(const std::string filename, const std::string name);

	void render() {
		for (auto &[k, r] : renderers) {
			glm::vec3 o{0.f};
			r->render(o);
		}

		for (auto &[k, model] : models) {
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_0)].tex);

			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_1)].tex);

			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_2)].tex);

			// Trick for test
			for (auto &[rendererName, renderer] : model->getRenderers()) {
				auto layerParams = renderer->getParams<LayersParams>("layers");
				if (layerParams) {
					layerParams->setColormapTexture(0, colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_0)]);
					layerParams->setColormapTexture(1, colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_1)]);
					layerParams->setColormapTexture(2, colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_2)]);
				}
			}

			model->render();
		}

		// Test
		for (auto &[shaderName, shader] : _shaders) {

			for (auto &[nodeName, node] : _nodes) {

				if (!node->isVisible())
					continue;

				auto shaderBufferOpt = node->getShaderBuffer(*shader);

				if (!shaderBufferOpt.has_value())
					continue;

				auto &shaderBuffer = shaderBufferOpt.value().get();

				if (node->getGeometry().shouldUpdate()) {
					// Update current shader buffers for given geometry
					shader->update(shaderBuffer, node->getGeometry());
				}

				glBindVertexArray(shaderBuffer.vao());
				shaderBuffer.setPosition(node->getWorldPosition());
				shaderBuffer.apply();

				// Set textures
				for (auto &tbo : shaderBuffer.tbos) {
					glActiveTexture(GL_TEXTURE0 + tbo.texUnit);
					glBindTexture(GL_TEXTURE_BUFFER, tbo.tex);
					shader->getShader().setInt(tbo.name, tbo.texUnit);
				}

				glDrawArrays(shader->renderElement(), 0, shaderBuffer.nelements);

			}

		}

		for (auto &[nodeName, node] : _nodes) {
			node->getGeometry().updateDone();
		}

	}

	void clean() {
		for (auto &[k, model] : models) {
			model->clean();
		}

		for (auto &[k, renderer] : renderers) {
			renderer->clean();
		}

		// Clear textures
		for (int i = 0; i < colormaps.size(); ++i)
			glDeleteTextures(1, &colormaps[i].tex);
	}

	void clear() {
		renderers.clear();
		models.clear();
		setSelectedModel("");
		cameras.clear();
		setupCameras();
		clearColormaps();
	}

	std::shared_ptr<Model> loadModel(const std::string& filename, std::string name = "");



	ModelCollection& getModels() { return models; }


	inline std::string getSelectedModel() {
		return selectedModel;
	}

	bool setSelectedModel(std::string name) {
		if (name.empty())
			return false;

		if (!models.has(name)) {
			std::cerr << "Invalid model selection: " << name << std::endl;
			return false;
		}

		auto oldSelection = selectedModel;
		selectedModel = name;
		// TODO important reactivate this !
		// notifySelectedModelChanged(oldSelection, name);
		return true;
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

	void focus(std::string modelName);
	void focus2(std::string nodeName);


	inline Model& getCurrentModel() {
		return *models[selectedModel];
	}

	inline std::shared_ptr<SceneNode> getCurrentNode() {
		return _nodes[selectedNode];
	}

	std::shared_ptr<Model> getHoveredModel();
	std::shared_ptr<SceneNode> getHoveredNode();


	std::tuple<glm::vec3, glm::vec3> computeSceneBBox();
	float computeSceneDiameter();
	void computeFarPlane();


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

	RendererCollection& getRenderers() { return renderers; }


	void setupColormaps();

	void addColormap(const std::string name, const std::string filename);
	void removeColormap(const std::string name);

	void clearColormaps() {
		colormaps.clear();
		setupColormaps();
	}

	std::vector<Colormap> getColormaps() {
		return colormaps;
	}

	Colormap getColormap(const std::string name);
	Colormap getColormap(int idx);

	void loadState(json &j, const std::string filename);
	void saveState(json &j, const std::string filename);

	RenderSurface& getDefaultRenderSurface() { return *renderSurfaces["default"]; }
	std::map<std::string, std::shared_ptr<RenderSurface>>& getRenderSurfaces() { return renderSurfaces; }

	const std::map<std::string, std::shared_ptr<SceneNode>>& getNodes() const {
		return _nodes;
	}

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
		for (auto &[nodeName, node] : _nodes) {
			if (node->getIndex() == index)
				return node;
		}

		return nullptr;
	}


	private:
	IApp &app;

	std::string selectedNode = "";

	std::string selectedModel = "";
	ModelCollection models;

	std::string selectedCamera = "default";
	CameraCollection cameras;

	RendererCollection renderers;

	std::map<std::string, std::shared_ptr<SceneNode>> _nodes;
	std::map<std::string, std::unique_ptr<Material>> _shaders;

	// display color map in good format for 2D in the UI
	std::vector<Colormap> colormaps;

	std::map<std::string, std::shared_ptr<RenderSurface>> renderSurfaces;
	
};