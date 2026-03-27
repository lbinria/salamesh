#pragma once
#include "camera.h"
#include "../entity_collection.h"

#include <map>
#include <memory>

struct CameraCollection : public EntityCollection<Camera> {

	std::shared_ptr<Camera> add(std::string type, std::string name) override {
		assert(!name.empty() && "Cannot add camera with an empty name.");

		// Check whether renderer already exists
		if (entities.contains(name))
			return entities[name];

		auto camera = instanciator.make(type, name);

		if (!camera)
			return nullptr;

		// camera->init();
		entities[name] = std::move(camera);
		return entities[name];
	}

	void remove(std::string name) override {
		// if (entities.count(name) > 0)
			// entities[name]->clean();

		entities.erase(name);
	}

	void clear() override {
		entities.clear();
		// setupCameras();
	}

};