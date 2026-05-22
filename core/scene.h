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
#include "renderers/point_style_params.h" // TODO remove

#include <map>

class IApp;

struct Scene {
	
	Scene(IApp &app) : app(app) {}


	void init() {
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
		renderers.getInstanciator().registerType("LineMaterial", [](std::string name) { return std::make_unique<LineMaterial>(name); });
		renderers.getInstanciator().registerType("PointMaterial", [](std::string name) { return std::make_unique<PointMaterial>(name); });

		// Init default render surface
		auto renderSurface = std::make_shared<RenderSurface>(1024, 768);
		renderSurface->setBackgroundColor({0.05, 0.1, 0.15});
		renderSurface->setup(); 
		renderSurfaces["default"] = std::move(renderSurface);

		setupCameras();

		getDefaultRenderSurface().setCamera(cameras["default"]);


		// Test
		cameras["default"]->lookAtBox({{-1,-1,-1}, {1,1,1}});

		auto pointMat = std::make_unique<PointMaterial>("points");
		auto surfaceMat = std::make_unique<TriMaterial>("tri");

		auto geo = std::make_unique<TrianglesGeometry>();
		geo->_m.points.create_points(3);
		geo->_m.create_facets(1);
		geo->_m.points[0] = {0.,0.,0.};
		geo->_m.points[1] = {1.,0.,0.};
		geo->_m.points[2] = {0.5,0.5,0.};
		geo->_m.vert(0, 0) = 0;
		geo->_m.vert(0, 1) = 1;
		geo->_m.vert(0, 2) = 2;

		SceneNode node;
		node.addShader(*pointMat);
		node.setGeometry(std::move(geo));
		nodes.emplace("node_1", std::move(node));

		auto geo2 = std::make_unique<TrianglesGeometry>();
		geo2->_m.points.create_points(3);
		geo2->_m.create_facets(1);
		geo2->_m.points[0] = {0.2,0.,0.};
		geo2->_m.points[1] = {0.8,0.,0.};
		geo2->_m.points[2] = {0.3,0.2,0.};
		geo2->_m.vert(0, 0) = 0;
		geo2->_m.vert(0, 1) = 1;
		geo2->_m.vert(0, 2) = 2;

		SceneNode node2;
		node2.addShader(*pointMat);
		node2.addShader(*surfaceMat);
		node2.setGeometry(std::move(geo2));
		auto sb = node2.getShaderBuffer("points");
		auto ps = sb.value().get().getParams<PointStyleParams>("style");
		ps->size = 10.f;
		ps->color = {1.f, 0.4f, 0.2f};

		nodes.emplace("node_2", std::move(node2));



		materials.emplace("points", std::move(pointMat));
		materials.emplace("tri", std::move(surfaceMat));
	}

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

			model->render();
		}

		// Test
		for (auto &[matName, mat] : materials) {

			for (auto &[nodeName, node] : nodes) {

				auto shaderBufferOpt = node.getShaderBuffer(*mat);

				if (!shaderBufferOpt.has_value())
					continue;

				auto &shaderBuffer = shaderBufferOpt.value().get();

				if (node.getGeometry().shouldUpdate()) {
					// Update current shader buffers for given geometry
					mat->update(shaderBuffer, node.getGeometry());
				}

				glBindVertexArray(shaderBuffer.vao());
				shaderBuffer.setPosition(node.position);
				shaderBuffer.apply();

				glDrawArrays(mat->renderElement(), 0, shaderBuffer.nelements);

			}

		}

		for (auto &[nodeName, node] : nodes) {
			node.getGeometry().updateDone();
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

	private:
	IApp &app;

	std::string selectedModel = "";
	ModelCollection models;

	std::string selectedCamera = "default";
	CameraCollection cameras;

	RendererCollection renderers;

	std::map<std::string, SceneNode> nodes;
	std::map<std::string, std::unique_ptr<Material>> materials;

	// display color map in good format for 2D in the UI
	std::vector<Colormap> colormaps;

	std::map<std::string, std::shared_ptr<RenderSurface>> renderSurfaces;
	
};