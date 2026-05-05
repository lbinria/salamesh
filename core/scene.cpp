#include "scene.h"
#include "app_interface.h"
#include "point_shader.h"

#include "utils/opengl_helper.h"

std::shared_ptr<TrianglesGeometry> loadTrianglesGeometry(const std::string &filename) {
	auto geometry = std::make_shared<TrianglesGeometry>();

	try {

		auto &m = geometry->getMesh();
		auto attr = read_by_extension(filename, m);

		if (m.nfacets() <= 0)
			return nullptr;
		
		int fs = m.facet(0).size();

		return geometry;

	} catch (std::runtime_error &ex) {
		std::cerr << "Unable to read " << filename << ": " << ex.what() << std::endl;
		return nullptr;
	}
}

// template<typename TMesh>
// std::shared_ptr<Geometry> loadGeometry(const std::string &filename) {
// 	TMesh m;

// 	try {

// 		auto attr = read_by_extension(filename, m);
// 		if (m.nfacets() <= 0)
// 			return nullptr;

		
// 		int fs = m.facet(0).size();

// 		// Check TMesh is Polygon
// 		bool isPoly = false;
// 		if constexpr (std::is_same_v<TMesh, Polygons>) {
// 			// N vert per facet greater than quad
// 			if (fs > 4) {
// 				isPoly = true;
// 			} else {
// 				// Or irregular number of vertices
// 				for (auto &f : m.iter_facets()) {
// 					if (fs != f.size())
// 						isPoly = true;
// 						break;
// 				}
// 			}

// 			if (!isPoly)
// 				return nullptr;
// 		}

// 		return std::make_shared<SurfaceGeometry>(std::move(m));

// 	} catch (std::runtime_error &ex) {
// 		std::cerr << "Unable to read " << filename << ": " << ex.what() << std::endl;
// 		return nullptr;
// 	}
// }

std::shared_ptr<SceneNode> Scene::load(const std::string& filename, const std::string name) {

	std::string modelName = name.empty() ? 
		std::filesystem::path(filename).stem().string() + std::to_string(models.count()) : 
		name;

	// auto geometry = loadGeometry<Triangles>(filename);
	// if (!geometry) {
	// 	geometry = loadGeometry<Quads>(filename);
	// }

	auto geometry = loadTrianglesGeometry(filename);

	auto ss = std::dynamic_pointer_cast<SurfaceGeometry>(geometry);
	auto &s= ss->getSurface();
	auto bf= s.nfacets();

	// Create default materials
	auto pointMaterial = _shaderPasses["point_shader_pass"]->createMaterial();
	// Create scene node
	auto node = std::make_shared<SceneNode>(name);
	node->setGeometry(geometry);
	node->addMaterial("point", pointMaterial);

	return node;


	// // We can load Triangles / Quads as Polygons, it works, i know
	// // but there is maybe some optimisations for Triangles, Quads...
	// // Note: check this improve the loading time of big models
	// if (getModelType() == ModelType::POLYGON_MODEL && !mustLoadedAsPoly()) {
	// 	return false;
	// }
	// clearAttrs();
	// auto containers = getAttributeContainers();
	// for (auto &[k, c] : containers) {
	// 	addAttr(k, c);
	// }
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
	auto meshRenderer = model->getMeshRenderer();
	if (meshRenderer) {
		meshRenderer->setMeshShrink(0.f);
		meshRenderer->setMeshSize(0.0f);
	}
	
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

void Scene::focus(std::string modelName) {
	setSelectedModel(modelName);
	auto &model = models[modelName];
	getCurrentCamera().lookAtBox(model->bbox());
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