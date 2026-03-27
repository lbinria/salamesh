#pragma once
#include "renderer.h"
#include "../entity_collection.h"

#include <map>
#include <memory>

struct RendererCollection : public EntityCollection<Renderer> {

	std::shared_ptr<Renderer> add(std::string type, std::string name) override {
		assert(!name.empty() && "Cannot add renderer with an empty name.");

		// Check whether renderer already exists
		if (entities.contains(name))
			return entities[name];

		auto renderer = instanciator.make(type, name);

		if (!renderer)
			return nullptr;

		renderer->init();
		entities[name] = std::move(renderer);
		return entities[name];
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