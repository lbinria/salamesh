#include "scene.h"
#include "app_interface.h"

#include "utils/opengl_helper.h"

void Scene::init() {
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
	auto polyMat = std::make_unique<PolyMaterial>("poly");
	auto lineShader = std::make_unique<LineMaterial>("line_shader");

	auto geo = std::make_unique<TrianglesGeometry>();
	geo->_m.points.create_points(3);
	geo->_m.create_facets(1);
	geo->_m.points[0] = {0.,0.,0.};
	geo->_m.points[1] = {1.,0.,0.};
	geo->_m.points[2] = {0.5,0.5,0.};
	geo->_m.vert(0, 0) = 0;
	geo->_m.vert(0, 1) = 1;
	geo->_m.vert(0, 2) = 2;

	auto node = std::make_shared<SceneNode>();
	node->addShader(*pointMat);
	node->setGeometry(std::move(geo));
	_nodes.emplace("node_1", std::move(node));

	auto geo2 = std::make_unique<TrianglesGeometry>();
	geo2->_m.points.create_points(3);
	geo2->_m.create_facets(1);
	geo2->_m.points[0] = {0.2,0.,0.};
	geo2->_m.points[1] = {0.8,0.,0.};
	geo2->_m.points[2] = {0.3,0.2,0.};
	geo2->_m.vert(0, 0) = 0;
	geo2->_m.vert(0, 1) = 1;
	geo2->_m.vert(0, 2) = 2;

	auto node2 = std::make_shared<SceneNode>();
	node2->addShader(*pointMat);
	node2->addShader(*surfaceMat);
	node2->setGeometry(std::move(geo2));
	auto sb = node2->getShaderBuffer("points");
	auto ps = sb.value().get().getParams<PointStyleParams>("style");
	ps->size = 10.f;
	ps->color = {1.f, 0.4f, 0.2f};

	_nodes.emplace("node_2", std::move(node2));

	_shaders.emplace("points", std::move(pointMat));
	_shaders.emplace("tri", std::move(surfaceMat));
	_shaders.emplace("poly", std::move(polyMat));
	_shaders.emplace("line_shader", std::move(lineShader));

	// loadModel2("assets/catorus_tri.geogram", "catorus");
	// loadModel2("assets/catorus_quad.geogram", "catorus");
	// loadModel2("assets/simple_poly.geogram", "catorus");
}

std::shared_ptr<Model> Scene::loadModel(const std::string& filename, std::string name) {

	auto begin = std::chrono::steady_clock::now();

	std::string modelName = name.empty() ? 
		std::filesystem::path(filename).stem().string() + std::to_string(models.count()) : 
		name;

	bool success = false;

	std::unique_ptr<Model> model;
	model = std::make_unique<PolyModel>(modelName);

	success = model->load(filename);

	if (!success) {
		model = std::make_unique<TriModel>(modelName);
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<QuadModel>(modelName);
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<TetModel>(modelName);
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<HexModel>(modelName);
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<PolylineModel>(modelName);
		success = model->load(filename);
	}

	if (!success)
		return nullptr;


	// Setup default gfx
	model->setLight(true);
	// auto meshRenderer = model->getMeshRenderer();
	// if (meshRenderer) {
	// 	meshRenderer->setMeshShrink(0.f);
	// 	meshRenderer->setMeshSize(0.0f);
	// }
	
	// auto edges = model->getEdgesRenderer();
	// if (edges && model->getModelType() == ModelType::POLYLINE_MODEL) {
	// 	edges->setVisible(true);
	// }

	// // By default points not visible
	// model->getPointsRenderer().setVisible(false);

	// Setup default clipping plane
	model->setupClipping();
	models[modelName] = std::move(model);

	// Update cameras far planes
	// computeFarPlane();
	computeFarPlane();


	// // Notify scripts
	// for (auto &s : scripts) {
	// 	s->modelLoaded(modelName);
	// }

	// A model was loaded ? focus it !
	if (!modelName.empty())
		focus(modelName);

	auto end = std::chrono::steady_clock::now();
	std::cout << "load model total duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;

	return models[modelName];
}

std::shared_ptr<SceneNode> Scene::loadModel2(const std::string filename, const std::string name) {

	std::string nodeName = name.empty() ? 
		std::filesystem::path(filename).stem().string() + std::to_string(models.count()) : 
		name;
		
	// Mesh node
	auto node = std::make_shared<SceneNode>(ModelLoader::load(filename));

	// Put all compatible shaders on model
	for (auto &[shaderName, shader] : _shaders) {
		if (shader->isCompatible(node->getGeometry()))
			node->addShader(*shader);
	}

	// node->getShaderBuffer("points").value().get().getParams("light")->set("enabled", false);

	_nodes.emplace(nodeName, std::move(node));

	// // BBox
	// auto bboxNode = std::make_shared<SceneNode>();
	// bboxNode->addShader(*_shaders.at("line_shader"));
	
	// auto bbox = node->bbox();
	// auto &lineGeo = bboxNode->createGeometry<LinesGeometry>();
	// lineGeo.addLine({ .a = {0.,0.,0.}, .b = {1.,0.,0.}, .color = {1., 1., 1.}});

	// node->add(bboxNode);
	// _nodes.emplace(name + "_bbox", std::move(bboxNode));

	// A model was loaded ? focus it !
	if (!nodeName.empty())
		focus2(nodeName);

	return _nodes.at(nodeName);
}

void Scene::focus(std::string modelName) {
	setSelectedModel(modelName);
	auto &model = models[modelName];
	getCurrentCamera().lookAtBox(model->bbox());
}

void Scene::focus2(const std::string nodeName) {
	if (!selectNode(nodeName))
		return;
	
	auto bbox = _nodes.at(nodeName)->bbox();
	getCurrentCamera().lookAtBox(bbox);
}

std::tuple<glm::vec3, glm::vec3> Scene::computeSceneBBox() {
	glm::vec3 min{std::numeric_limits<float>::max()};
	glm::vec3 max{-std::numeric_limits<float>::max()};
	for (auto &[k, m] : models) {
		auto [cmin, cmax] = m->bbox();
		min = glm::min(min, cmin);
		max = glm::max(max, cmax);
	}
	
	return std::make_tuple(min, max);
}

float Scene::computeSceneDiameter() {
	auto [min, max] = computeSceneBBox();
	return glm::length(max - min);
}

void Scene::computeFarPlane() {
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
	addColormap("cat", "/home/tex/Models/cat/Cat_diffuse.jpg");
	addColormap("extended", sl::assetsPath("extended.png"));
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

void Scene::loadState(json &j, const std::string filename) {
	// Load models states
	for (auto &[modelName, jModel] : j["models"].items()) {
		// Concatenate state.json file path with model path
		// in order to search the mesh file relatively to the state.json file
		std::string modelRelPath = jModel["path"];
		auto modelPath = 
			std::filesystem::path(filename).remove_filename() / 
			std::filesystem::path(modelRelPath);
		
		// Try to load the model mesh
		if (!loadModel(modelPath.string(), modelName))
			continue;
		
		// Get last added model
		auto &model = models[modelName];
		// Load state into last loaded model
		model->loadState(jModel);

		// TODO! recompute cameras far / near
	}

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
	std::filesystem::path p = filename;

	j["selected_model"] = selectedModel;
	j["selected_camera"] = selectedCamera;
	j["models"] = json::object();
	j["cameras"] = json::object();

	// Save models states
	for (auto &[k, m] : models) {
		m->saveState(p.parent_path().string(), j["models"][k]);
	}

	// Save cameras states
	for (auto &[k, c] : cameras) {
		c->saveState(j["cameras"][k]);
	}

	// TODO important save renderer states
	// TODO important save colormaps states
}

std::shared_ptr<Model> Scene::getHoveredModel() {
	auto hoveredIndex = app.getInputState().mesh.getHovered();
	return models.getByIndex(hoveredIndex);
}

std::shared_ptr<SceneNode> Scene::getHoveredNode() {
	auto hoveredIndex = app.getInputState().mesh.getHovered();
	return getNodeByIndex(hoveredIndex);;
}