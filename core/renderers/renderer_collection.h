#pragma once
#include "renderer.h"

#include <map>
#include <memory>

struct RendererCollection {

template<typename T>
struct Instanciator {

	Instanciator() = default;

	// Instanciator (const Instanciator&) = delete;
	// Instanciator& operator= (const Instanciator&) = delete;

	std::unique_ptr<T> make(std::string type) {
		if (instanciators.count(type) > 0) {
			return instanciators[type]();
		}

		std::cerr 
			<< "Unable to make entity of type " 
			<< type 
			<< ", maybe you should register your custom entity class using `registerType` ?" 
			<< std::endl;

		return nullptr;
	}
	
	inline void registerType(std::string type, std::function<std::unique_ptr<T>()> instanciatorFunc) {
		instanciators[type] = instanciatorFunc;
	}

	std::vector<std::string> listAvailableTypes() {
		std::vector<std::string> v;
		for (auto &[k, r] : instanciators)
			v.push_back(k);

		return v;
	}

	private:
	// Instanciator, enable entity instanciation
	// Register new instanciator for custom entity
	std::map<std::string, std::function<std::unique_ptr<T>()>> instanciators;
};

	auto begin() {
		return renderers.begin();
	}

	auto end() {
		return renderers.end();
	}

	auto begin() const {
		return renderers.begin();
	}

	auto end() const {
		return renderers.end();
	}

	std::shared_ptr<Renderer>& operator[](const std::string& name) {
		return renderers[name];
	}

	const std::shared_ptr<Renderer>& operator[](const std::string& name) const {
		return renderers.at(name);
	}

	std::shared_ptr<Renderer> add(std::string type, std::string name) {
		assert(!name.empty() && "Cannot add renderer with an empty name.");

		// Check whether renderer already exists
		if (renderers.contains(name))
			return renderers[name];

		auto renderer = instanciator.make(type);

		if (!renderer)
			return nullptr;

		renderer->init();
		renderers[name] = std::move(renderer);
		return renderers[name];
	}

	void remove(std::string name) {
		if (renderers.count(name) > 0)
			renderers[name]->clean();

		renderers.erase(name);
	}

	int count() {
		return renderers.size();
	}

	bool has(std::string name) {
		return renderers.count(name) > 0;
	}

	bool any() {
		return renderers.size() > 0;
	}
	
	void clear() {
		for (auto &[k, r] : renderers) {
			r->clean();
		}

		renderers.clear();
	}

	Renderer& get(std::string name) {
		if (renderers.count(name) <= 0)
			throw std::runtime_error("Renderer " + name + " was not found.");
		
		return *renderers[name];
	}

	std::map<std::string, std::shared_ptr<Renderer>>& get() {
		return renderers;
	}

	Instanciator<Renderer>& getInstanciator() { return instanciator; }

	private:
	std::map<std::string, std::shared_ptr<Renderer>> renderers;
	Instanciator<Renderer> instanciator;


};