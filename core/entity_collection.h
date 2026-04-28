#pragma once

#include "instanciator.h"
#include "scene.h"

#include <map>
#include <memory>

struct Scene;

template<typename T>
struct EntityCollection {

	EntityCollection(Scene &scene) : scene(scene) {

	}

	auto begin() {
		return entities.begin();
	}

	auto end() {
		return entities.end();
	}

	auto begin() const {
		return entities.begin();
	}

	auto end() const {
		return entities.end();
	}

	std::shared_ptr<T>& operator[](const std::string& name) {
		return entities[name];
	}

	const std::shared_ptr<T>& operator[](const std::string& name) const {
		return entities.at(name);
	}

	virtual std::shared_ptr<T> add(std::string type, std::string name) = 0;
	virtual void remove(std::string name) = 0;
	virtual void clear() = 0;

	std::map<std::string, std::shared_ptr<T>>& get() {
		return entities;
	}

	T& get(std::string name) {
		if (entities.count(name) <= 0)
			throw std::runtime_error("T " + name + " was not found.");
		
		return *entities[name];
	}

	int count() {
		return entities.size();
	}

	bool has(std::string name) {
		return entities.count(name) > 0;
	}

	bool any() {
		return entities.size() > 0;
	}

	Instanciator<T>& getInstanciator() { return instanciator; }

	protected:
	std::map<std::string, std::shared_ptr<T>> entities;
	Instanciator<T> instanciator;

	private:
	Scene &scene;

};