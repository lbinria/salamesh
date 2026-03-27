#pragma once
#include "renderer.h"
#include "../entity_collection.h"

#include <map>
#include <memory>

struct RendererCollection : public EntityCollection<Renderer> {

	std::shared_ptr<Renderer> add(std::string type, std::string name) override {

		// Check whether renderer already exists
		if (entities.contains(name))
			return entities[name];

		auto renderer = instanciator.make(type, name);

		if (!renderer)
			return nullptr;

		auto n = renderer->getName();

		renderer->init();
		entities[n] = std::move(renderer);
		return entities[n];
	}

	void remove(std::string name) override {
		if (entities.count(name) > 0)
			entities[name]->clean();

		entities.erase(name);
	}
	
	void clear() override {
		for (auto &[k, r] : entities) {
			r->clean();
		}

		entities.clear();
	}

};