#include "scene.h"
#include "app_interface.h"

#include "shaders/point_shader.h"
#include "shaders/tri_shader.h"
#include "shaders/poly_shader.h"
#include "shaders/halfedge_shader.h"
#include "shaders/line_shader.h"

#include "utils/opengl_helper.h"

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

	// Register node type


	// Register cameras types
	cameras.getInstanciator().registerType("DescentCamera", [](std::string name) { return std::make_unique<DescentCamera>(name); });
	cameras.getInstanciator().registerType("TrackBallCamera", [](std::string name) { return std::make_unique<TrackBallCamera>(name); });

	// Init default render surface
	auto renderSurface = std::make_shared<RenderSurface>(1024, 768);
	renderSurface->setBackgroundColor({0.05, 0.1, 0.15});
	renderSurface->setup(); 
	renderSurfaces["default"] = std::move(renderSurface);

	setupCameras();

	getDefaultRenderSurface().setCamera(cameras["default"]);

	auto pointsShader = std::make_unique<PointShader>("points");
	auto trianglesShader = std::make_unique<TriShader>("mesh");
	auto polygonsShader = std::make_unique<PolyShader>("mesh");
	auto halfedgesShader = std::make_unique<HalfedgeShader>("halfedges");
	auto lineShader = std::make_unique<LineShader>("line_shader");

	_shaders.emplace("points_shader", std::move(pointsShader));
	_shaders.emplace("a_triangles_shader", std::move(trianglesShader));
	_shaders.emplace("polygons_shader", std::move(polygonsShader));
	_shaders.emplace("line_shader", std::move(lineShader));
	_shaders.emplace("halfedges_shader", std::move(halfedgesShader));

}

std::shared_ptr<SceneNode> Scene::loadModel(const std::string filename, const std::string name) {
	std::string nodeName = name.empty() ? 
		std::filesystem::path(filename).stem().string() + std::to_string(countNodes()) : 
		name;

	// Load node from file
	auto modelLoader = ModelLoader(*this);
	auto node = modelLoader.load(filename, name);

	// Setup default gfx
	node->getMaterial("points").value().get().setVisible(false);
	node->getMaterial("halfedges").value().get().setVisible(false);

	_nodes.emplace(nodeName, std::move(node));

	// Update scene far plane
	updateFarPlane();

	// A model was loaded ? focus it !
	if (!nodeName.empty())
		focus(nodeName);

	return node;
}

void Scene::focus(const std::string nodeName) {
	if (!setSelectedNode(nodeName))
		return;
	
	auto bbox = _nodes.at(nodeName)->bbox();
	getCurrentCamera().lookAtBox(bbox);
}

std::tuple<glm::vec3, glm::vec3> Scene::computeSceneBBox() {
	glm::vec3 min{std::numeric_limits<float>::max()};
	glm::vec3 max{-std::numeric_limits<float>::max()};
	for (auto &[_, n] : _nodes) {
		auto [cmin, cmax] = n->bbox();
		min = glm::min(min, cmin);
		max = glm::max(max, cmax);
	}
	
	return std::make_tuple(min, max);
}

float Scene::computeSceneDiameter() {
	auto [min, max] = computeSceneBBox();
	return glm::length(max - min);
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

	for (const auto& cm : colormaps) {
		if (cm.name == name) {
			std::cerr << "App::addColormap: colormap '" << name << "' already exists." << std::endl;
			return;
		}
	}

	int width, height, nrChannels;
	
	Colormap cm{
		name,
		0,
		0,
		0
	};

	if(!sl::load_texture_2d(filename, cm.tex, width, height, nrChannels)) {
		std::cerr << "App::addColormap: unable to load colormap " << name << " at " << filename << "." << std::endl;
		return;
	}

	cm.width = width;
	cm.height = height;

	colormaps.push_back(cm);
}

void Scene::removeColormap(const std::string name) {
	for (int i = 0; i < colormaps.size(); ++i) {
		if (colormaps[i].name == name) {
			colormaps.erase(colormaps.begin() + i);
			return;
		}
	}
}

Colormap Scene::getColormap(const std::string name) {
	for (int i = 0; i < colormaps.size(); ++i) {
		if (colormaps[i].name == name) {
			return colormaps[i];
		}
	}
	throw std::runtime_error("Colormap " + name + " not found.");
}

Colormap Scene::getColormap(int idx) {
	return colormaps[idx];
}

void Scene::render(std::shared_ptr<SceneNode> node, std::unique_ptr<ShaderBase>& shader, std::map<std::string, bool> &wasUpdated) {
	if (!node->isVisible())
		return;

	for (auto &child : node->getChildren()) {
		render(child, shader, wasUpdated);
	}

	auto shaderBufferOpt = node->getShaderBuffer(*shader);
	auto materialOpt = node->getMaterial(shader->getName());

	if (!shaderBufferOpt.has_value() || !materialOpt.has_value())
		return;

	auto &shaderBuffer = shaderBufferOpt.value().get();
	auto &material = materialOpt.value().get();

	// TODO: maybe we can delay update shader buffer when material is not visible
	if (node->getGeometry().shouldUpdate()) {
		// Update current shader buffers for given geometry
		shader->update(shaderBuffer, node->getGeometry());
		// Update layers (only activated layers) according to new geometry
		node->updateLayers();
		// Set node as updated
		wasUpdated[node->getName()] = true;
	}

	if (!material.isVisible())
		return;

	glBindVertexArray(shaderBuffer.vao());
	shaderBuffer.setPosition(node->getWorldPosition());
	material.apply(shader->getShader());

	// Set textures
	for (auto &tbo : shaderBuffer.tbos) {
		glActiveTexture(GL_TEXTURE0 + tbo.texUnit);
		glBindTexture(GL_TEXTURE_BUFFER, tbo.tex);
		shader->getShader().setInt(tbo.name, tbo.texUnit);
	}

	// Set mesh index
	shader->getShader().setInt("meshIndex", node->getIndex());

	glDrawArrays(shader->renderElement(), 0, shaderBuffer.nelements);
}

void Scene::render() {

	// Keep updated nodes in memory
	std::map<std::string, bool> wasUpdated;

	// Loop through available shaders
	for (auto &[shaderName, shader] : _shaders) {
		// Loop through nodes in scene
		for (auto &[nodeName, node] : _nodes) {
			render(node, shader, wasUpdated);
		}

	}

	for (auto &node : getNodesAndDescendants()) {
		if (wasUpdated.contains(node->getName())) {
			node->getGeometry().updateDone();
		}
	}

}

void Scene::loadState(json &j, const std::string filename) {
	// Load models states
	// for (auto &[modelName, jModel] : j["models"].items()) {
	// 	// Concatenate state.json file path with model path
	// 	// in order to search the mesh file relatively to the state.json file
	// 	std::string modelRelPath = jModel["path"];
	// 	auto modelPath = 
	// 		std::filesystem::path(filename).remove_filename() / 
	// 		std::filesystem::path(modelRelPath);
		
	// 	// Try to load the model mesh
	// 	if (!loadModel(modelPath.string(), modelName))
	// 		continue;
		
	// 	// Get last added model
	// 	auto &model = models[modelName];
	// 	// Load state into last loaded model
	// 	model->loadState(jModel);

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

	setSelectedNode(j["selected_model"].get<std::string>());
	setSelectedCamera(j["selected_camera"].get<std::string>());
}

void Scene::saveState(json &j, const std::string filename) {
	std::filesystem::path p = filename;

	// j["selected_model"] = selectedModel;
	j["selected_camera"] = selectedCamera;
	j["models"] = json::object();
	j["cameras"] = json::object();

	// // Save models states
	// for (auto &[k, m] : models) {
	// 	m->saveState(p.parent_path().string(), j["models"][k]);
	// }

	// Save cameras states
	for (auto &[k, c] : cameras) {
		c->saveState(j["cameras"][k]);
	}

	// TODO important save renderer states
	// TODO important save colormaps states
}

std::shared_ptr<SceneNode> Scene::getHoveredNode() {
	auto hoveredIndex = app.getInputState().mesh.getHovered();
	return findNodeByIndex(hoveredIndex);;
}