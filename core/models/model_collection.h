#pragma once
#include "model.h"
#include "../entity_collection.h"

#include <map>
#include <memory>

struct ModelCollection : public EntityCollection<Model> {

	std::shared_ptr<Model> add(std::string type, std::string name) override {
		// TODO maybe check type exists, else we dont understand why we get null model ?

		// Check whether renderer already exists
		if (entities.contains(name))
			return entities[name];

		auto model = instanciator.make(type, name);

		if (!model)
			return nullptr;
		
		auto n = model->getName();
		
		model->init();
		entities[n] = std::move(model);
		return entities[n];

		// Recompute far plane
	}

	void remove(std::string name) override {
		if (entities.contains(name))
			entities[name]->clean();

		entities.erase(name);
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
		Model::clearIndex();
	}

	std::shared_ptr<Model> getByIndex(int index) {
		for (auto &[k, e] : entities) {
			if (e->getIndex() == index)
				return e;
		}

		return nullptr;
	}

};