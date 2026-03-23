#pragma once
#include "model.h"

#include <map>
#include <memory>

struct ModelCollection {

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
		return models.begin();
	}

	auto end() {
		return models.end();
	}

	auto begin() const {
		return models.begin();
	}

	auto end() const {
		return models.end();
	}

	std::shared_ptr<Model>& operator[](const std::string& name) {
		return models[name];
	}

	const std::shared_ptr<Model>& operator[](const std::string& name) const {
		return models.at(name);
	}


	std::shared_ptr<Model> add(std::string type, std::string name) {
		assert(!name.empty() && "Cannot add model with an empty name.");

		// TODO maybe chekc type exists, else we dont understand why we get null model ?

		// Check whether renderer already exists
		if (models.contains(name))
			return models[name];

		auto model = instanciator.make(type);

		if (!model)
			return nullptr;
		
		model->init();
		modelNameByIndex[model->getIndex()] = name;
		models[name] = std::move(model);
		return models[name];

		// // 
		// // model->loadCallback = ([this](Model&, const std::string) -> bool {
		// // 	this->computeFarPlane();
		// // 	this->script->modelLoaded(blabla)
		// // });
	}

	void remove(std::string name) {
		if (models.count(name) > 0)
			models[name]->clean();

		models.erase(name);
		// Sync
		for (auto &[i, curName] : modelNameByIndex) {
			if (curName == name) {
				modelNameByIndex.erase(i);
				break;
			}
		}
	}
	
	Model& get(std::string name) {
		if (models.count(name) <= 0)
			throw std::runtime_error("Model " + name + " was not found.");
		
		return *models[name];
	}

	std::map<std::string, std::shared_ptr<Model>>& get() {
		return models;
	}

	std::vector<std::shared_ptr<Model>> getChildrenOf(std::shared_ptr<Model> model) {
		std::vector<std::shared_ptr<Model>> children;
		for (auto &[k, m] : models) {
			if (m->getParent() == model) {
				children.push_back(m);
			}
		}

		return children;
	}

	inline int count() {
		return models.size();
	}

	inline bool has(std::string name) {
		return models.count(name) > 0;
	}

	inline bool any() {
		return models.size() > 0;
	}

	void clear() {
		for (auto &[k, m] : models) {
			m->clean();
		}

		models.clear();
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


	Instanciator<Model>& getInstanciator() { return instanciator; }

	std::map<int, std::string> modelNameByIndex;

	private:
	std::map<std::string, std::shared_ptr<Model>> models;
	Instanciator<Model> instanciator;


};