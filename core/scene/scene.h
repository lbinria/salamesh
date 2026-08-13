#pragma once

#include "colormap.h"
#include "input_states.h"

#include "../render_surface.h"

#include "camera.h"
#include "trackball_camera.h"
#include "descent_camera.h"
#include "camera_collection.h"


#include "shader_base.h"

#include "scene_model.h"
#include "../model_loader.h"// TODO remove test


#include <map>

class IApp;

struct Scene {
	
	Scene(IApp &app) : app(app) {}


	void init();
	std::shared_ptr<SceneModel> loadModel(const std::string filename, const std::string name = "");

	void render();


	void clean() {
		cleanupModels();
		cleanupShaders();
		cleanupColormaps();
		cleanupRenderSurfaces();
	}

	void clear() {
		// Keep shaders
		// Keep render surfaces for now
		setSelectedModel("");

		cleanupModels();
		_models.clear();

		resetCameras();
		resetColormaps();
	}

	void cleanupModels() {
		for (auto &[_, model] : _models)
			model->clean();
	}

	void cleanupColormaps() {
		for (auto &[_, colormap] : colormaps)
			glDeleteTextures(1, &colormap.tex);
	}

	void cleanupShaders() {
		for (auto &[_, shader] : _shaders)
			shader->clean();
	}

	void cleanupRenderSurfaces() {
		for (auto &[_, renderSurface] : renderSurfaces)
			renderSurface->clean();
	}

	bool setSelectedModel(std::string name) {
		if (name.empty()) {
			selectedModel = "";
			return false;
		}

		if (!_models.contains(name)) {
			std::cerr << "Invalid model selection: " << name << std::endl;
			return false;
		}

		auto oldSelection = selectedModel;
		selectedModel = name;
		// TODO important reactivate this !
		// notifySelectedModelChanged(oldSelection, name);
		return true;
	}

	const std::string getSelectedModel() {
		return selectedModel;
	}

	void focus(std::string modelName);

	inline std::shared_ptr<SceneModel> getCurrentModel() {
		if (!selectedModel.empty())
			return _models.at(selectedModel);

		return nullptr;
	}

	// std::optional<std::reference_wrapper<Mesh>>  getHoveredMesh();


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

	void resetCameras() {
		cameras.clear();
		setupCameras();
	}

	void resetColormaps() {
		cleanupColormaps();
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


	bool hasModels() const {
		return _models.size() > 0;
	}

	int countModels() const {
		return static_cast<int>(_models.size());
	}

	std::shared_ptr<SceneModel> createModel(const std::string name, std::shared_ptr<Mesh> mesh) {
		auto model = std::make_shared<SceneModel>(name, mesh);
		_models.emplace(model->getName(), model);
		return model;
	}

	const std::vector<std::shared_ptr<SceneModel>> getModels() const {
		std::vector<std::shared_ptr<SceneModel>> result;
		for (auto &[_, model] : _models)
			result.push_back(model);
		
		return result;
	}

	std::shared_ptr<SceneModel> findModelByName(const std::string name) {
		return _models.contains(name) ? _models.at(name) : nullptr;
	}

	std::optional<std::reference_wrapper<Mesh>> findMeshByIndex(int index) {
		for (auto &[_, model] : _models) {
			auto &mesh = model->getMesh();
			if (mesh.getIndex() == index)
				return mesh;
		}

		return std::nullopt;
	}

	// TODO add findModelByMesh

	const std::map<std::string, std::unique_ptr<ShaderBase>>& getShaders() const {
		return _shaders;
	}

	std::optional<std::reference_wrapper<ShaderBase>> getShader(const std::string name) const {
		if (_shaders.contains(name)) {
			return std::ref(*_shaders.at(name));  // Dereference unique_ptr
		}
		return std::nullopt;
	}

	private:
	IApp &app;

	std::string selectedModel = "";

	std::string selectedCamera = "default";
	CameraCollection cameras;

	std::map<std::string, std::shared_ptr<SceneModel>> _models;
	std::map<std::string, std::unique_ptr<ShaderBase>> _shaders;

	// display color map in good format for 2D in the UI
	std::map<std::string, Colormap> colormaps;

	std::map<std::string, std::shared_ptr<RenderSurface>> renderSurfaces;
	
};