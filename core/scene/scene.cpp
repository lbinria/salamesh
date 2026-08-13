#include "scene.h"
#include "../app_interface.h"

#include "point_shader.h"
#include "triangles_shader.h"
#include "tetrahedras_shader.h"
#include "poly_shader.h"
#include "halfedge_shader.h"
#include "line_shader.h"

#include "opengl_helper.h"

void Scene::init() {
	// Register model types
	// models.getInstanciator().registerType("TriModel", [](std::string name) { return std::make_unique<TriModel>(name); });
	// models.getInstanciator().registerType("QuadModel", [](std::string name) { return std::make_unique<QuadModel>(name); });
	// models.getInstanciator().registerType("PolyModel", [](std::string name) { return std::make_unique<PolyModel>(name); });
	// models.getInstanciator().registerType("TetModel", [](std::string name) { return std::make_unique<TetModel>(name); });
	// models.getInstanciator().registerType("HexModel", [](std::string name) { return std::make_unique<HexModel>(name); });
	// models.getInstanciator().registerType("PolylineModel", [](std::string name) { return std::make_unique<PolylineModel>(name); });
	// models.getInstanciator().registerType("PyramidModel", [](std::string name) { return std::make_unique<PyramidModel>(name); });
	// models.getInstanciator().registerType("PrismModel", [](std::string name) { return std::make_unique<PrismModel>(name); });

	// Register model type


	// Register cameras types
	// cameras.getInstanciator().registerType("DescentCamera", [](std::string name) { return std::make_unique<DescentCamera>(name); });
	cameras.getInstanciator().registerType("TrackBallCamera", [](std::string name) { return std::make_unique<TrackBallCamera>(name); });

	// Init default render surface
	auto renderSurface = std::make_shared<RenderSurface>(1024, 768);
	renderSurface->setBackgroundColor({0.16, 0.17, 0.18});
	renderSurface->setup(); 
	renderSurfaces["default"] = std::move(renderSurface);

	setupCameras();

	getDefaultRenderSurface().setCamera(cameras["default"]);

	auto pointsShader = std::make_unique<PointShader>("points");
	auto trianglesShader = std::make_unique<TrianglesShader>("mesh");
	auto tetrahedrasShader = std::make_unique<TetrahedrasShader>("mesh");
	auto polygonsShader = std::make_unique<PolyShader>("mesh");
	auto halfedgesShader = std::make_unique<HalfedgeShader>("halfedges");
	auto lineShader = std::make_unique<LineShader>("line_shader");

	_shaders.emplace("point_shader", std::move(pointsShader));
	_shaders.emplace("a_triangle_shader", std::move(trianglesShader));
	_shaders.emplace("a_polygon_shader", std::move(polygonsShader));
	_shaders.emplace("line_shader", std::move(lineShader));
	_shaders.emplace("halfedge_shader", std::move(halfedgesShader));
	_shaders.emplace("a_tetrahedras_shader", std::move(tetrahedrasShader));

}

std::shared_ptr<SceneModel> Scene::loadModel(const std::string filename, const std::string name) {
	std::string modelName = name.empty() ? 
		std::filesystem::path(filename).stem().string() + std::to_string(countModels()) : 
		name;

	// Load model from file
	auto modelLoader = ModelLoader(*this);
	auto model = modelLoader.load(filename, modelName);

	if (!model)
		return nullptr;

	// Setup default gfx for surfaces
	if (model->hasMaterial("mesh")) {
		if (auto mat = model->getMaterial("points")) {
			mat->get().setVisible(false);
		}

		if (auto mat = model->getMaterial("halfedges")) {
			mat->get().setVisible(false);
		}
	}

	_models.emplace(modelName, model);

	// Update scene far plane
	updateFarPlane();

	// A model was loaded ? focus it !
	if (!modelName.empty())
		focus(modelName);

	return model;
}

void Scene::focus(const std::string modelName) {
	if (!setSelectedModel(modelName))
		return;
	
	auto bbox = _models.at(modelName)->getMesh().bbox();
	getCurrentCamera().lookAtBox(bbox);
}

std::tuple<vec3, vec3> Scene::computeSceneBBox() {
	vec3 min{std::numeric_limits<float>::max()};
	vec3 max{-std::numeric_limits<float>::max()};
	for (auto &[_, model] : _models) {
		auto [cmin, cmax] = model->getMesh().bbox();
		min = sl::min(min, cmin);
		max = sl::max(max, cmax);
	}
	
	return std::make_tuple(min, max);
}

float Scene::computeSceneDiameter() {
	auto [min, max] = computeSceneBBox();
	return (max - min).norm2();
}

void Scene::updateFarPlane() {
	auto diameter = computeSceneDiameter();

	for (auto &[k, c] : cameras) {
		c->setFarPlane(diameter * 5.f /* 5.f is an arbitrary value... */);
	}

	// TODO should refresh camera here, elsewhere nothing will be visible until we doing somethin that refresh the camera!
}

void Scene::setupCameras() {
	auto trackballCamera = std::make_shared<TrackBallCamera>("default");
	cameras["default"] = std::move(trackballCamera);
	setSelectedCamera("default");
}

void Scene::setupColormaps() {
	// Load default colormap textures
	addColormap("CET-R41", sl::assetsPath("CET-R41px.png"));
	addColormap("CET-L08", sl::assetsPath("CET-L08px.png"));
	addColormap("alpha", sl::assetsPath("colormap_alpha.png"));
	// addColormap("cat", "/home/tex/Models/cat/Cat_diffuse.jpg");
}

void Scene::addColormap(const std::string name, const std::string filename) {

	if (colormaps.contains(name)) {
		std::cerr << "Scene::addColormap: colormap '" << name << "' already exists." << std::endl;
		return;
	}

	int width, height, nrChannels;
	
	Colormap cm{name, 0, 0, 0};

	if(!sl::load_texture_2d(filename, cm.tex, cm.width, cm.height, nrChannels)) {
		std::cerr << "Scene::addColormap: unable to load colormap " << name << " at " << filename << "." << std::endl;
		return;
	}

	colormaps.emplace(name, cm);
}

void Scene::removeColormap(const std::string name) {
	// Free vram, not for the moment because dangerous to gain a little of memory
	// (because of Colormap is copyable and contains unsigned int tex pointer)
	// if (colormaps.contains(name)) {
	// 	auto &colormap = colormaps.at(name);
	// 	glDeleteTextures(1, &colormap.tex);
	// }

	colormaps.erase(name);
}

Colormap Scene::getColormap(const std::string name) {
	if (!colormaps.contains(name))
		throw std::runtime_error("Colormap " + name + " not found.");

	return colormaps.at(name);
}

void Scene::render() {

	// Keep updated models in memory
	std::map<std::string, bool> wasUpdated;

	// Loop through available shaders
	for (auto &[shaderId, shader] : _shaders) {
		// Loop through models in scene
		for (auto &[modelName, model] : _models) {
			
			if (!model->isVisible())
				continue;

			// auto meshBufferOpt = model->getMeshBuffer(shader->getName());
			auto meshBufferOpt = model->getMeshBuffer(shaderId);
			auto materialOpt = model->getMaterial(shader->getName());

			if (!meshBufferOpt.has_value() || !materialOpt.has_value())
				continue;

			auto &mesh = model->getMesh();
			auto &meshBuffer = meshBufferOpt.value().get();

			if (mesh.shouldUpdate()) {
				switch (meshBuffer.streams()) {
					case MeshBuffer::Stream::POINTS_STREAM: {
						auto stream = mesh.getPointsStream();
						meshBuffer.nelements = stream.size();
						meshBuffer.write(stream);
						break;
					};
					case MeshBuffer::Stream::EDGES_STREAM: {
						auto stream = mesh.getEdgesStream();
						meshBuffer.nelements = stream.size();
						meshBuffer.write(stream);
						break;
					};
					case MeshBuffer::Stream::TRIANGLES_STREAM: {
						auto stream = mesh.getTrianglesStream();
						meshBuffer.nelements = stream.size();
						meshBuffer.write(stream);
						break;
					};
				}

				// Check for data streams
				for (auto &[dataName, data] : mesh.getDataStreams()) {
					meshBuffer.write(dataName, data);
				}

				// TODO important can optimize this loop, it enter as many times as there is shader attached to model, there is no need to pass each time here !!!

				model->layers.update();
				// Set model as updated
				wasUpdated[model->getName()] = true;
			}

			auto &material = materialOpt.value().get();

			if (!material.isVisible())
				continue;

			glBindVertexArray(meshBuffer.vao());
			meshBuffer.setPosition(shader->getShader(), model->position);
			material.apply(shader->getShader());
			model->layers.apply(shader->getShader());

			// Set textures
			for (auto &[_, tbo] : meshBuffer.tbos) {
				glActiveTexture(GL_TEXTURE0 + tbo.texUnit);
				glBindTexture(GL_TEXTURE_BUFFER, tbo.tex);
				shader->getShader().setInt(tbo.name, tbo.texUnit);
			}

			// Set mesh index
			shader->getShader().setInt("meshIndex", mesh.getIndex());

			glDrawArrays(shader->renderElement(), 0, meshBuffer.nelements);
		}

	}

	for (auto &[_, model] : _models) {
		if (wasUpdated.contains(model->getName())) {
				model->getMesh().updateDone();
		}
	}

}

void Scene::loadState(json &j, const std::string filename) {
	// Load models states
	// for (auto &[modelName, jModel] : j["models"].items()) {

	// 	auto model = createModel(modelName);
	// 	model->loadState(jModel, filename);
	// 	// TODO! recompute cameras far / near
	// }

	// Load cameras states after model (because loading model will focus on)
	for (auto &[cameraName, jCamera] : j["cameras"].items()) {
		auto type = jCamera["type"].get<std::string>();
		auto camera = cameras.getInstanciator().make(type, cameraName);
		if (camera) {
			camera->loadState(jCamera);
			cameras[cameraName] = std::move(camera);
		}
	}

	setSelectedModel(j["selected_model"].get<std::string>());
	setSelectedCamera(j["selected_camera"].get<std::string>());
}

void Scene::saveState(json &j, const std::string filename) {

	j["selected_model"] = selectedModel;
	j["selected_camera"] = selectedCamera;
	j["models"] = json::object();
	j["cameras"] = json::object();

	// Save models states
	for (auto &[k, model] : _models) {
		model->saveState(j["models"][k], filename);
	}

	// Save cameras states
	for (auto &[k, c] : cameras) {
		c->saveState(j["cameras"][k]);
	}

	// TODO important save colormaps states
}

// std::optional<std::reference_wrapper<Mesh>> Scene::getHoveredMesh() {
// 	auto hoveredIndex = app.getInputState().mesh.getHovered();
// 	return findMeshByIndex(hoveredIndex);;
// }