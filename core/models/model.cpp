#include "model.h"
#include "model_view.h"

void Model::push() {

	for (auto const &[k, r] : _renderers)
		r->push();

	// Update model views
	for (auto &[viewName, mv] : views) {
		mv.push();
	}
}

bool Model::saveState(std::string dirPath, json &j) /*const*/ {

	// Save current mesh state into a file
	// auto now = std::chrono::system_clock::now();
	// auto unix_timestamp = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
	// auto filename = _name + "_" + unix_timestamp + ".geogram";
	auto guid = sl::generateGuid();
	// auto filename = guid + "_" + unix_timestamp + ".geogram";
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


	j["visible"] = visible;

	for (auto &[viewName, mv] : views) {
		mv.saveState(j);
	}


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
	


	for (auto &[viewName, mv] : views) {
		mv.loadState(j);
	}



	setVisible(j["visible"].get<bool>());

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

ModelView Model::getDefaultView(std::string viewName) {

	// Get / create (if not exists) default views of renderers
	std::map<std::string, std::shared_ptr<RendererView>> rendererViews;
	for (auto &[k, r] : _renderers) {
		rendererViews[k] = r->getView(viewName);
	}

	ModelView mv(*this, rendererViews);
	mv.visible = true;
	mv.setLightEnabled(true);
	


	auto meshRenderer = getMeshRenderer();
	if (meshRenderer) {
		auto rv = meshRenderer->getView(viewName);
		auto mrv = std::static_pointer_cast<MeshRendererView>(rv);
		mrv->setMeshIndex(index);
	}

	return mv;
}

ModelView& Model::getView(std::string viewName) {
	// Create default view
	if (!views.contains(viewName))
		views.insert({viewName, getDefaultView(viewName)});

	return views.at(viewName);
}

void Model::render(ModelView &modelView) {

	if (!modelView.visible)
		return;

	glm::vec3 pos = getWorldPosition();

	for (auto const &[k, r] : _renderers) {
		auto rv = modelView.getRendererView(k);
		r->render(*rv, pos);
	}
}