#pragma once
#include "core/app_interface.h"
#include "core/scene_interface.h"
#include "core/models/model.h"
#include "core/models/surface_model.h"
#include "core/models/volume_model.h"
#include "core/models/polyline_model.h"
#include "core/cameras/trackball_camera.h"
#include "core/cameras/descent_camera.h"

struct Scene : public IScene {
	
	Scene(IApp &app) : app(app) {}


	void init() override {
		// Register model types
		models.getInstanciator().registerType("TriModel", []() { return std::make_unique<TriModel>(); });
		models.getInstanciator().registerType("QuadModel", []() { return std::make_unique<QuadModel>(); });
		models.getInstanciator().registerType("PolyModel", []() { return std::make_unique<PolyModel>(); });
		models.getInstanciator().registerType("TetModel", []() { return std::make_unique<TetModel>(); });
		models.getInstanciator().registerType("HexModel", []() { return std::make_unique<HexModel>(); });
		// models.getInstanciator().registerType("PolylineModel", []() { return std::make_unique<PolylineModel>(); });
		// models.getInstanciator().registerType("PyramidModel", []() { return std::make_unique<PyramidModel>(); });
		// models.getInstanciator().registerType("PrismModel", []() { return std::make_unique<PrismModel>(); });

		// Register cameras types
		cameras.getInstanciator().registerType("DescentCamera", []() { return std::make_unique<DescentCamera>(); });
		cameras.getInstanciator().registerType("TrackBallCamera", []() { return std::make_unique<TrackBallCamera>(); });

		// TODO register renderer

		// Create cameras
		setupCameras();

		app.getRenderSurface().setCamera(cameras["default"]);
	}

	void clean() override {
		for (auto &[k, model] : models) {
			model->clean();
		}
	}

	void clear() override {
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
		// notifySelectedModelChanged(oldSelection, name);
		return true;
	}

	void focus(std::string modelName) override;


	inline Model& getCurrentModel() override {
		return *models[selectedModel];
	}

	std::shared_ptr<Model> getHoveredModel() {
		// Searching
		// auto hoveredIndex = st.mesh.getHovered();
		auto hoveredIndex = app.getInputState().mesh.getHovered();

		
		if (models.hasModelIndex(hoveredIndex)) {
			auto name = models.getModelNameByIndex(hoveredIndex);
			if (models.has(name)) {
				return models[name];
			}
		}

		return nullptr;
	}

	void setupCameras();


	bool setSelectedCamera(std::string selected) override {
		if (selected.empty())
			return false;

		if (!cameras.has(selected)) {
			std::cerr << "Invalid camera selection: " << selected << std::endl;
			return false;
		}

		// Set camera to render surface
		app.getRenderSurface().setCamera(cameras[selected]);
		selectedCamera = selected;
		return true;
	}
	
	std::string getSelectedCamera() override {
		return selectedCamera;
	}

	Camera& getCurrentCamera() override { return *cameras[selectedCamera]; }
	CameraCollection& getCameras() override { return cameras; }

	private:
	IApp &app;

	std::string selectedModel = "";
	ModelCollection models;

	std::string selectedCamera = "default";
	CameraCollection cameras;


};