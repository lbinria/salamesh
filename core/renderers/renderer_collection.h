#pragma once
#include "renderer.h"
#include "../entity_collection.h"

#include <map>
#include <memory>

struct RendererCollection : public EntityCollection<Renderer> {

	using EntityCollection::EntityCollection;


	std::shared_ptr<Renderer> add(std::string type, std::string name) override {

		// Check whether renderer already exists
		if (entities.contains(name))
			return entities[name];

		auto renderer = instanciator.make(type, name);

		if (!renderer)
			return nullptr;

		auto n = renderer->getName();

		entities[n] = std::move(renderer);
		return entities[n];
	}

	void remove(std::string name) override {
		entities.erase(name);
	}
	
	void clear() override {
		entities.clear();
	}

};