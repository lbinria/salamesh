#pragma once
#include "material_instance.h"
#include "model_state.h"
#include "cameras/camera.h"
#include "models/model.h"
#include "renderers/renderer.h"
#include "render_surface.h"



struct SceneView {

	SceneView(std::string name, int width, int height) : name(name) {}

	std::string getName() const {
		return name;
	}

	void clean() {
		// Clean materials
		for (auto &[matName, mat] : materials) {
			mat.clean();
		}
	}

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

	std::map<std::string, MaterialInstance> materials; // material by renderer
	std::map<std::string, ModelState> modelStates; // model state by model

};