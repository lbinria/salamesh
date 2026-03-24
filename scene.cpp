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
	// getCurrentCamera().lookAtBox(model->bbox());
	app.getCurrentCamera().lookAtBox(model->bbox());
}
