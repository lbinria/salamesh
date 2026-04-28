#pragma once
#include "app_interface.h"
#include "scene_view.h"

#include "cameras/trackball_camera.h"
#include "cameras/descent_camera.h"
#include "renderers/line_renderer.h"

#include "models/model.h"
#include "models/surface_model.h"
#include "models/volume_model.h"
#include "models/polyline_model.h"

#include "renderers/renderer.h"

#include "cameras/camera_collection.h"
#include "models/model_collection.h"
#include "renderers/renderer_collection.h"

#include "graphic_api.h"

#include <map>

struct IApp;

struct Scene {
	
	Scene(IApp &app) : app(app), models(*this), cameras(*this), renderers(*this) {}


	void init();

	void clean();

	void clear();

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

	void focus(std::string modelName);


	inline Model& getCurrentModel() {
		return *models[selectedModel];
	}

	std::shared_ptr<Model> getHoveredModel();

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

		// Set camera to render surfaces (same camera for all for now)
		for (auto &[renderSurfaceName, renderSurface] : renderSurfaces) {
			renderSurface->setCamera(cameras[selected]);
		}

		selectedCamera = selected;
		return true;
	}
	
	std::string getSelectedCamera() {
		return selectedCamera;
	}

	Camera& getCurrentCamera() { return *cameras[selectedCamera]; }
	CameraCollection& getCameras() { return cameras; }

	RendererCollection& getRenderers() { return renderers; }




	void loadState(json &j, const std::string filename);
	void saveState(json &j, const std::string filename);



	SceneView& getMainView() { return *views["default"]; }
	SceneView& getCurrentView() { return *views[selectedView]; }
	
	std::map<std::string, std::shared_ptr<SceneView>>& getViews() { return views; }

	SceneView& addView(const std::string name) {
		// Init default view
		auto view = std::make_shared<SceneView>(name, 1024, 768);
		views[name] = std::move(view);
		return *views[name];
	}

	std::string getSelectedView() {
		return selectedView;
	}

	bool setSelectedView(const std::string name) {
		if (name.empty())
			return false;

		if (!views.contains(name)) {
			std::cerr << "Invalid view selection: " << name << std::endl;
			return false;
		}

		selectedView = name;
		return true;
	}

	RenderSurface &getRenderSurface() { return *renderSurfaces["default"]; }

	private:
	IApp &app;

	std::string selectedView = "default";

	std::string selectedModel = "";
	ModelCollection models;

	std::string selectedCamera = "default";
	CameraCollection cameras;

	RendererCollection renderers;

	std::map<std::string, std::shared_ptr<SceneView>> views;
	std::map<std::string, std::shared_ptr<RenderSurface>> renderSurfaces;
	


};