#pragma once
#include "camera.h"

#include <map>
#include <memory>

struct CameraCollection {

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
		return cameras.begin();
	}

	auto end() {
		return cameras.end();
	}

	auto begin() const {
		return cameras.begin();
	}

	auto end() const {
		return cameras.end();
	}

	std::shared_ptr<Camera>& operator[](const std::string& name) {
		return cameras[name];
	}

	const std::shared_ptr<Camera>& operator[](const std::string& name) const {
		return cameras.at(name);
	}

	std::shared_ptr<Camera> add(std::string type, std::string name) {
		assert(!name.empty() && "Cannot add camera with an empty name.");

		// Check whether renderer already exists
		if (cameras.contains(name))
			return cameras[name];

		auto camera = instanciator.make(type);

		if (!camera)
			return nullptr;

		// camera->init();
		cameras[name] = std::move(camera);
		return cameras[name];
	}

	void remove(std::string name) {
		// if (cameras.count(name) > 0)
			// cameras[name]->clean();

		cameras.erase(name);
	}

	void clear() {
		cameras.clear();
		// setupCameras();
	}

	std::map<std::string, std::shared_ptr<Camera>>& get() {
		return cameras;
	}

	Camera& get(std::string name) {
		if (cameras.count(name) <= 0)
			throw std::runtime_error("Camera " + name + " was not found.");
		
		return *cameras[name];
	}

	int count() {
		return cameras.size();
	}

	bool has(std::string name) {
		return cameras.count(name) > 0;
	}

	bool any() {
		return cameras.size() > 0;
	}

	Instanciator<Camera>& getInstanciator() { return instanciator; }

	private:
	std::map<std::string, std::shared_ptr<Camera>> cameras;
	Instanciator<Camera> instanciator;


};