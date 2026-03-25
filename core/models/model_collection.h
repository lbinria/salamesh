#pragma once
#include "model.h"
#include "../entity_collection.h"

#include <map>
#include <memory>

struct ModelCollection : public EntityCollection<Model> {

	std::shared_ptr<Model> add(std::string type, std::string name) override {
		assert(!name.empty() && "Cannot add model with an empty name.");

		// TODO maybe chekc type exists, else we dont understand why we get null model ?

		// Check whether renderer already exists
		if (entities.contains(name))
			return entities[name];

		auto model = instanciator.make(type);

		if (!model)
			return nullptr;
		
		model->init();
		modelNameByIndex[model->getIndex()] = name;
		entities[name] = std::move(model);
		return entities[name];

		// // 
		// // model->loadCallback = ([this](Model&, const std::string) -> bool {
		// // 	this->computeFarPlane();
		// // 	this->script->modelLoaded(blabla)
		// // });
	}

	void remove(std::string name) override {
		if (entities.count(name) > 0)
			entities[name]->clean();

		entities.erase(name);
		// Sync
		for (auto &[i, curName] : modelNameByIndex) {
			if (curName == name) {
				modelNameByIndex.erase(i);
				break;
			}
		}
	}
	
	std::vector<std::shared_ptr<Model>> getChildrenOf(std::shared_ptr<Model> model) {
		std::vector<std::shared_ptr<Model>> children;
		for (auto &[k, m] : entities) {
			if (m->getParent() == model) {
				children.push_back(m);
			}
		}

		return children;
	}

	void clear() override {
		for (auto &[k, m] : entities) {
			m->clean();
		}

		entities.clear();
		modelNameByIndex.clear();
		Model::clearIndex();
	}

	bool hasModelIndex(int index) {
		return modelNameByIndex.count(index) > 0;
	}

	std::string getModelNameByIndex(int index) {
		return modelNameByIndex[index];
	}

	int getModelIndexByName(std::string name) {
		for (auto &[i, modelName] : modelNameByIndex) {
			if (modelName == name) {
				return i;
			}
		}
		return -1;
	}

	std::map<int, std::string> modelNameByIndex;

};