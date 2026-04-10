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

	MaterialInstance& getMaterial(Renderer &renderer) {
		return materials.at(renderer.getId());
	}

	MaterialInstanceCollection getMaterials(const Model &model) {
		std::map<std::string, std::string> rendererInfos;
		for (auto &[k, r] : model.getRenderers()) {
			rendererInfos[r->getType()] = r->getId();
		}
		return MaterialInstanceCollection(materials, rendererInfos);
	}

	bool hasMaterial(const Renderer &renderer) const {
		return materials.contains(renderer.getId());
	}

	void addMaterial(const Renderer &renderer, const MaterialInstance mat) {
		materials[renderer.getId()] = mat;
	}
	
	bool removeMaterial(const Renderer &renderer) {
		return materials.erase(renderer.getId());
	}



	private:
	std::string name;
	glm::vec3 backgroundColor{0.05, 0.1, 0.15};
	RenderSurface renderSurface;

	std::map<std::string, MaterialInstance> materials;


};