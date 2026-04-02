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
	
	Scene(IApp &app) : app(app) {}


	void init() override {
		// Register model types
		models.getInstanciator().registerType("TriModel", [](std::string name) { return std::make_unique<TriModel>(name); });
		models.getInstanciator().registerType("QuadModel", [](std::string name) { return std::make_unique<QuadModel>(name); });
		models.getInstanciator().registerType("PolyModel", [](std::string name) { return std::make_unique<PolyModel>(name); });
		models.getInstanciator().registerType("TetModel", [](std::string name) { return std::make_unique<TetModel>(name); });
		models.getInstanciator().registerType("HexModel", [](std::string name) { return std::make_unique<HexModel>(name); });
		// models.getInstanciator().registerType("PolylineModel", [](std::string name) { return std::make_unique<PolylineModel>(name); });
		// models.getInstanciator().registerType("PyramidModel", [](std::string name) { return std::make_unique<PyramidModel>(name); });
		// models.getInstanciator().registerType("PrismModel", [](std::string name) { return std::make_unique<PrismModel>(name); });

		// Register cameras types
		cameras.getInstanciator().registerType("DescentCamera", [](std::string name) { return std::make_unique<DescentCamera>(name); });
		cameras.getInstanciator().registerType("TrackBallCamera", [](std::string name) { return std::make_unique<TrackBallCamera>(name); });

		// Register renderers types
		renderers.getInstanciator().registerType("LineRenderer", [](std::string name) { return std::make_unique<LineRenderer>(name); });
		renderers.getInstanciator().registerType("PointSetRenderer", [](std::string name) { return std::make_unique<PointSetRenderer>(name); });

		// Init default view
		auto view = std::make_shared<SceneView>("default", 1024, 768);
		view->setup();
		views[view->getName()] = std::move(view);

		setupCameras();

		// TODO replace by getMainView.setCamera
		getMainView().getRenderSurface().setCamera(cameras["default"]);
	}

	void render() override {
		for (auto &[k, r] : renderers) {
			glm::vec3 o{0.f};
			r->render(o);
		}

		for (auto &[k, model] : models) {
			// For each views
			// Get modelview associated with model
			// use shaders of modelview & render model
			// model->render(modelView);
			
			model->setColormap0Texture(colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_0)].tex);
			model->setColormap1Texture(colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_1)].tex);
			model->setColormap2Texture(colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_2)].tex);
			model->render();
		}

		// Render view
		for (auto &[viewName, view] : views) {
			for (auto &[modelName, model] : models) {
				// Get view of the model
				// auto &mv = view->getModelView(*model);
				auto &mv = model->getView(viewName);
				model->render(mv);
			}
		}

	}

	void clean() override {
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

	void clear() override {
		renderers.clear();
		models.clear();
		setSelectedModel("");
		cameras.clear();
		setupCameras();
		clearColormaps();
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


	void setupColormaps();

	void addColormap(const std::string name, const std::string filename) override;
	void removeColormap(const std::string name) override;

	void clearColormaps() override {
		colormaps.clear();
		setupColormaps();
	}

	std::vector<Colormap> getColormaps() override {
		return colormaps;
	}

	Colormap getColormap(const std::string name) override;
	Colormap getColormap(int idx) override;

	void loadState(json &j, const std::string filename);
	void saveState(json &j, const std::string filename);

	ISceneView& getMainView() { return *views["default"]; }
	std::map<std::string, std::shared_ptr<ISceneView>>& getViews() { return views; }

	private:
	IApp &app;

	std::string selectedModel = "";
	ModelCollection models;

	std::string selectedCamera = "default";
	CameraCollection cameras;

	RendererCollection renderers;

	// display color map in good format for 2D in the UI
	std::vector<Colormap> colormaps;

	std::map<std::string, std::shared_ptr<ISceneView>> views;
	
};