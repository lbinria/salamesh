#pragma once
#include "core/app_interface.h"
#include "core/scene_interface.h"
#include "scene_view.h"

#include "core/models/model.h"
#include "core/models/surface_model.h"
#include "core/models/volume_model.h"
#include "core/models/polyline_model.h"
#include "core/cameras/trackball_camera.h"
#include "core/cameras/descent_camera.h"
#include "core/renderers/line_renderer.h"
#include "core/graphic_api.h"

#include <map>



struct Scene : public IScene {
	
	Scene(IApp &app) : app(app), models(*this), cameras(*this), renderers(*this) {}


	void init() override {
		// Register model types
		models.getInstanciator().registerType("TriModel", [](std::string name) { return std::make_unique<TriModel>(name); });
		models.getInstanciator().registerType("QuadModel", [](std::string name) { return std::make_unique<QuadModel>(name); });
		models.getInstanciator().registerType("PolyModel", [](std::string name) { return std::make_unique<PolyModel>(name); });
		models.getInstanciator().registerType("TetModel", [](std::string name) { return std::make_unique<TetModel>(name); });
		models.getInstanciator().registerType("HexModel", [](std::string name) { return std::make_unique<HexModel>(name); });
		models.getInstanciator().registerType("PolylineModel", [](std::string name) { return std::make_unique<PolylineModel>(name); });
		// models.getInstanciator().registerType("PyramidModel", [](std::string name) { return std::make_unique<PyramidModel>(name); });
		// models.getInstanciator().registerType("PrismModel", [](std::string name) { return std::make_unique<PrismModel>(name); });

		// Register cameras types
		cameras.getInstanciator().registerType("DescentCamera", [](std::string name) { return std::make_unique<DescentCamera>(name); });
		cameras.getInstanciator().registerType("TrackBallCamera", [](std::string name) { return std::make_unique<TrackBallCamera>(name); });

		// Register renderers types
		renderers.getInstanciator().registerType("LineRenderer", [](std::string name) { return std::make_unique<LineRenderer>(name); });
		renderers.getInstanciator().registerType("PointSetRenderer", [](std::string name) { return std::make_unique<PointSetRenderer>(name); });

		// Init default view
		addView("default");
		addView("test");

		setupCameras();

		// TODO replace by getMainView.setCamera
		getMainView().getRenderSurface().setCamera(cameras["default"]);
		views["test"]->getRenderSurface().setCamera(cameras["default"]);
	}

	void clean() override {
		// Clean models
		for (auto &[modelName, model] : models) {
			model->clean();
		}

		// Clean views
		for (auto &[viewName, view] : views) {
			view->clean();
		}
	}

	void clear() override {
		renderers.clear();
		models.clear();
		setSelectedModel("");
		cameras.clear();
		setupCameras();
	}

	std::shared_ptr<Model> loadModel(const std::string& filename, std::string name = "") override;



	ModelCollection& getModels() override { return models; }


	inline std::string getSelectedModel() override {
		return selectedModel;
	}

	bool setSelectedModel(std::string name) override {
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

	void focus(std::string modelName) override;


	inline Model& getCurrentModel() override {
		return *models[selectedModel];
	}

	std::shared_ptr<Model> getHoveredModel() {
		auto hoveredIndex = app.getInputState().mesh.getHovered();
		return models.getByIndex(hoveredIndex);
	}

	std::tuple<glm::vec3, glm::vec3> computeSceneBBox();
	float computeSceneDiameter();
	void computeFarPlane();


	void setupCameras();


	bool setSelectedCamera(std::string selected) override {
		if (selected.empty())
			return false;

		if (!cameras.has(selected)) {
			std::cerr << "Invalid camera selection: " << selected << std::endl;
			return false;
		}

		// Set camera to render surface
		getMainView().setCamera(cameras[selected]);
		selectedCamera = selected;
		return true;
	}
	
	std::string getSelectedCamera() override {
		return selectedCamera;
	}

	Camera& getCurrentCamera() override { return *cameras[selectedCamera]; }
	CameraCollection& getCameras() override { return cameras; }

	RendererCollection& getRenderers() override { return renderers; }




	void loadState(json &j, const std::string filename);
	void saveState(json &j, const std::string filename);



	ISceneView& getMainView() override { return *views["default"]; }
	ISceneView& getCurrentView() override { return *views[selectedView]; }
	
	std::map<std::string, std::shared_ptr<ISceneView>>& getViews() { return views; }

	ISceneView& addView(const std::string name) override {
		// Init default view
		auto view = std::make_shared<SceneView>(name, 1024, 768);
		view->setup();
		views[name] = std::move(view);
		return *views[name];
	}

	std::string getSelectedView() override {
		return selectedView;
	}

	bool setSelectedView(const std::string name) override {
		if (name.empty())
			return false;

		if (!views.contains(name)) {
			std::cerr << "Invalid view selection: " << name << std::endl;
			return false;
		}

		selectedView = name;
		return true;
	}

	private:
	IApp &app;

	std::string selectedView = "default";

	std::string selectedModel = "";
	ModelCollection models;

	std::string selectedCamera = "default";
	CameraCollection cameras;

	RendererCollection renderers;

	std::map<std::string, std::shared_ptr<ISceneView>> views;
	


};