#include "scene.h"

std::shared_ptr<Model> Scene::loadModel(const std::string& filename, std::string name) {

	auto begin = std::chrono::steady_clock::now();

	std::string modelName = name.empty() ? 
		std::filesystem::path(filename).stem().string() + std::to_string(models.count()) : 
		name;

	bool success = false;

	std::unique_ptr<Model> model;
	model = std::make_unique<PolyModel>();

	success = model->load(filename);

	if (!success) {
		model = std::make_unique<TriModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<QuadModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<TetModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<HexModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<PolylineModel>();
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
	models.modelNameByIndex[model->getIndex()] = modelName;
	models[modelName] = std::move(model);

	// Update cameras far planes
	// computeFarPlane();
	app.computeFarPlane();


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

void Scene::setupCameras() {
	auto trackballCamera = std::make_shared<TrackBallCamera>();
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