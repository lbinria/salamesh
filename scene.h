#pragma once
#include "core/app_interface.h"
#include "core/scene_interface.h"
#include "core/models/model.h"
#include "core/models/surface_model.h"
#include "core/models/volume_model.h"
#include "core/models/polyline_model.h"

struct Scene : public IScene {
	
	Scene(IApp &app) : app(app) {}


	void init() override {
		// Register model types
		models.getInstanciator().registerType("TriModel", []() { return std::make_unique<TriModel>(); });
		models.getInstanciator().registerType("QuadModel", []() { return std::make_unique<QuadModel>(); });
		models.getInstanciator().registerType("PolyModel", []() { return std::make_unique<PolyModel>(); });
		models.getInstanciator().registerType("TetModel", []() { return std::make_unique<TetModel>(); });
		models.getInstanciator().registerType("HexModel", []() { return std::make_unique<HexModel>(); });
		// models.getInstanciator().registerType("PolylineModel", []() { return std::make_unique<PolylineModel>(); });
		// models.getInstanciator().registerType("PyramidModel", []() { return std::make_unique<PyramidModel>(); });
		// models.getInstanciator().registerType("PrismModel", []() { return std::make_unique<PrismModel>(); });
	}

	void clean() override {
		for (auto &[k, model] : models) {
			model->clean();
		}
	}

	void clear() override {
		models.clear();
		setSelectedModel("");
	}

	std::shared_ptr<Model> loadModel(const std::string& filename, std::string name = "") override;



	ModelCollection& getModels() override { return models; }


	inline std::string getSelectedModel() override {
		return selectedModel;
	}

	bool setSelectedModel(std::string name) override {
		if (name.empty())
			return false;

		if (!models.has(name)) {
			std::cerr << "Invalid model selection: " << name << std::endl;
			return false;
		}

		auto oldSelection = selectedModel;
		selectedModel = name;
		// notifySelectedModelChanged(oldSelection, name);
		return true;
	}

	void focus(std::string modelName) override;


	inline Model& getCurrentModel() override {
		return *models[selectedModel];
	}

	std::shared_ptr<Model> getHoveredModel() {
		// Searching
		// auto hoveredIndex = st.mesh.getHovered();
		auto hoveredIndex = app.getInputState().mesh.getHovered();

		
		if (models.hasModelIndex(hoveredIndex)) {
			auto name = models.getModelNameByIndex(hoveredIndex);
			if (models.has(name)) {
				return models[name];
			}
		}

		return nullptr;
	}

	private:
	IApp &app;
	std::string selectedModel = "";
	ModelCollection models;


};