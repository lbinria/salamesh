#include "model.h"

bool Model::saveState(std::string dirPath, json &j) /*const*/ {

	// Save current mesh state into a file
	auto guid = sl::generateGuid();
	auto filename = guid + ".geogram";
	auto filepath = std::filesystem::path(dirPath) / filename;
	
	// When saving state 
	// we must keep / save all attributes of the mesh
	// For example if user filtered something, we would like to keep filter
	// So we add all mesh attributes to model
	clearAttrs();
	auto containers = getAttributeContainers();
	for (auto &[k, c] : containers)
		addAttr(k, c);

	if (!saveAs(filepath.string())) {
		std::cerr << "Unable to save state at " << filepath << std::endl;
		return false;
	}

	j["path"] = filename;
	j["position"] = { position.x, position.y, position.z };



	for (auto &[k, r] : _renderers) {
		r->saveState(j["renderers"][k]);
	}

	doSaveState(j);

	return true;
}

void Model::loadState(json &j) {
	
	_path = j["path"].get<std::string>();

	position = glm::vec3(
		j["position"][0].get<float>(),
		j["position"][1].get<float>(),
		j["position"][2].get<float>()
	);


	// for (auto &[viewName, mv] : views) {
	// 	mv.loadState(j["views"][viewName]);
	// }

	// Load renderers state
	auto renderers = getRenderers();

	for (auto &[k, r] : renderers) {
		if (j["renderers"].contains(k))
			r->loadState(j["renderers"][k]);
	}

	doLoadState(j);
}

void Model::addAttr(ElementKind kind, NamedContainer &container) {
	
	// Get the type of the container
	ElementType type = ElementType::DOUBLE_ELT; // Default type
	if (auto a = dynamic_cast<AttributeContainer<double>*>(container.ptr.get())) {
		type = ElementType::DOUBLE_ELT;
	} else if (auto a = dynamic_cast<AttributeContainer<int>*>(container.ptr.get())) {
		type = ElementType::INT_ELT;
	} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(container.ptr.get())) {
		type = ElementType::BOOL_ELT;
	} else if (auto a = dynamic_cast<AttributeContainer<vec2>*>(container.ptr.get())) {
		type = ElementType::VEC2_ELT;
		attrs.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
		attrs.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
	} else if (auto a = dynamic_cast<AttributeContainer<vec3>*>(container.ptr.get())) {
		type = ElementType::VEC3_ELT;
		attrs.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
		attrs.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
		attrs.emplace_back(container.name + "[2]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
	} else {
		throw std::runtime_error("Unknown attribute type for container: " + container.name);
	}

	attrs.emplace_back(container.name, kind, type, container.ptr, false);
}

void Model::removeAttr(int idx) {
	if (idx >= 0 && idx < attrs.size())
		attrs.erase(attrs.begin() + idx);
}

void Model::removeAttr(ElementKind kind, std::string name) {
	int idx = -1;
	for (int i = 0; i < attrs.size(); ++i) {
		if (attrs[i].name == name) {
			idx = i;
			break;
		}
	}

	if (idx >= 0)
		attrs.erase(attrs.begin() + idx);
}

