#pragma once

template<typename T>
struct Instanciator {

	Instanciator() = default;

	// Instanciator (const Instanciator&) = delete;
	// Instanciator& operator= (const Instanciator&) = delete;

	std::unique_ptr<T> make(std::string type, std::string name) {
		if (instanciators.count(type) > 0) {
			return instanciators[type](name);
		}

		std::cerr 
			<< "Unable to make entity of type " 
			<< type 
			<< ", maybe you should register your custom entity class using `registerType` ?" 
			<< std::endl;

		return nullptr;
	}
	
	inline void registerType(std::string type, std::function<std::unique_ptr<T>(std::string)> instanciatorFunc) {
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
	std::map<std::string, std::function<std::unique_ptr<T>(std::string)>> instanciators;
};