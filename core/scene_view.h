#pragma once
#include "material_instance.h"
#include "model_state.h"
#include "cameras/camera.h"
#include "models/model.h"
#include "renderers/renderer.h"
#include "render_surface.h"



struct SceneView {

	SceneView(std::string name, int width, int height) : name(name), renderSurface(width, height) {}

	std::string getName() const {
		return name;
	}

	// TODO pass width, height on setup, remove from constructor
	void setup() {
		renderSurface.setBackgroundColor(backgroundColor);
		renderSurface.setup(); 
	}

	void render(Shader &screenShader, unsigned int quadVAO) {
		renderSurface.render(screenShader, quadVAO);
	}

	void clear() {
		renderSurface.clear();
	}

	void resize(int w, int h) {
		renderSurface.resize(w, h);
	}

	void clean() {
		// Clean materials
		for (auto &[matName, mat] : materials) {
			mat.clean();
		}
		
		renderSurface.clean();
	}

	void setCamera(std::shared_ptr<Camera> camera) {
		renderSurface.setCamera(camera);
	}

	RenderSurface &getRenderSurface() { return renderSurface; }

	MaterialInstance& getMaterial(Renderer &renderer) {
		return materials.at(renderer.getId());
	}

	MaterialInstance& getMaterialOrDefault(Renderer &renderer) {
		if (!hasMaterial(renderer))
			addMaterial(renderer, renderer.getDefaultMaterial());

		return getMaterial(renderer);
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

	void addMaterial(const Renderer &renderer, MaterialInstance mat) {
		materials[renderer.getId()] = std::move(mat);
	}
	
	bool removeMaterial(const Renderer &renderer) {
		return materials.erase(renderer.getId());
	}

	ModelState& getState(Model &model) {
		if (!modelStates.contains(model.getName()))
			modelStates.emplace(model.getName(), ModelState());

		return modelStates.at(model.getName());
	}

	// SceneView& copy(const std::string name) {

	// }

	private:
	std::string name;
	glm::vec3 backgroundColor{0.05, 0.1, 0.15};
	RenderSurface renderSurface;

	std::map<std::string, MaterialInstance> materials; // material by renderer
	std::map<std::string, ModelState> modelStates; // model state by model

};