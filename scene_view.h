#pragma once
#include "core/scene_view_interface.h"
#include "core/material_instance.h"

struct SceneView : public ISceneView {

	SceneView(std::string name, int width, int height) : name(name), renderSurface(width, height) {}

	std::string getName() const {
		return name;
	}

	// TODO pass width, height on setup, remove from constructor
	void setup() override {
		renderSurface.setBackgroundColor(backgroundColor);
		renderSurface.setup(); 
	}

	void render(Shader &screenShader, unsigned int quadVAO) override {
		renderSurface.render(screenShader, quadVAO);
	}

	void clear() override {
		renderSurface.clear();
	}

	void resize(int w, int h) override {
		renderSurface.resize(w, h);
	}

	void clean() override {
		renderSurface.clean();
	}

	void setCamera(std::shared_ptr<Camera> camera) {
		renderSurface.setCamera(camera);
	}

	RenderSurface &getRenderSurface() override { return renderSurface; }

	ModelView& getModel(Model &model) override {
		// auto n = model.getName();
		// if (!models.contains(n)) {
		// 	// TODO Create default view
		// 	ModelView mv(model, );
		// 	models.insert({n, model.getDefaultView()});
		// }

		// return models.at(n);
	}

	MaterialInstance& getMaterial(const std::string objectName, const std::string vcName) {
		return materials[objectName][vcName];
	}

	private:
	std::string name;
	glm::vec3 backgroundColor{0.05, 0.1, 0.15};
	RenderSurface renderSurface;

	std::map<std::string, ModelView> models;

	std::map<std::string, std::map<std::string, MaterialInstance>> materials;

};