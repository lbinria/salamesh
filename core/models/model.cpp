#include "model.h"

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
	j["is_light_enabled"] = isLightEnabled;
	j["is_light_follow_view"] = isLightFollowView;
	j["is_clipping"] = isClipping;
	j["clipping_plane_point"] = { clippingPlanePoint.x, clippingPlanePoint.y, clippingPlanePoint.z };
	j["clipping_plane_normal"] = { clippingPlaneNormal.x, clippingPlaneNormal.y, clippingPlaneNormal.z };
	j["invert_clipping"] = invertClipping;
	// TODO save directly as arrays
	j["selected_colormap0"] = selectedColormap[0];
	j["selected_colormap1"] = selectedColormap[1];
	j["selected_colormap2"] = selectedColormap[2];
	j["selected_attr0"] = selectedAttr[0];
	j["selected_attr1"] = selectedAttr[1];
	j["selected_attr2"] = selectedAttr[2];
	j["visible"] = visible;

	j["attr_name_by_layer_and_kind"] = attrNameByLayerAndKind;
	j["activated_layers"] = activatedLayers;

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
	
	setLight(j["is_light_enabled"].get<bool>());
	setLightFollowView(j["is_light_follow_view"].get<bool>());
	setClipping(j["is_clipping"].get<bool>());

	setClippingPlanePoint(glm::vec3(
		j["clipping_plane_point"][0].get<float>(),
		j["clipping_plane_point"][1].get<float>(),
		j["clipping_plane_point"][2].get<float>()
	));

	setClippingPlaneNormal(glm::vec3(
		j["clipping_plane_normal"][0].get<float>(),
		j["clipping_plane_normal"][1].get<float>(),
		j["clipping_plane_normal"][2].get<float>()
	));

	setInvertClipping(j["invert_clipping"].get<bool>());

	// TODO load directly as array
	selectedAttr[0] = j["selected_attr0"].get<int>();
	selectedAttr[1] = j["selected_attr1"].get<int>();
	selectedAttr[2] = j["selected_attr2"].get<int>();
		
	selectedColormap[0] = j["selected_colormap0"].get<int>();
	selectedColormap[1] = j["selected_colormap1"].get<int>();
	selectedColormap[2] = j["selected_colormap2"].get<int>();

	for (auto &j : j["attr_name_by_layer_and_kind"]) {
		auto key = j[0];
		Layer l = (Layer)key[0].get<int>();
		ElementKind k = (ElementKind)key[1].get<int>();
		auto attrName = j[1].get<std::string>();
		attrNameByLayerAndKind[{l, k}] = attrName;
	}

	// Set layers
	for (auto &j : j["activated_layers"]) {
		auto key = j[0];
		Layer l = (Layer)key[0].get<int>();
		ElementKind k = (ElementKind)key[1].get<int>();
		auto val = j[1].get<bool>();
		if (val) {
			setLayer(k, l);
		}
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
		attrs.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, 0);
		attrs.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, 1);
	} else if (auto a = dynamic_cast<AttributeContainer<vec3>*>(container.ptr.get())) {
		type = ElementType::VEC3_ELT;
		attrs.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, 0);
		attrs.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, 1);
		attrs.emplace_back(container.name + "[2]", kind, ElementType::DOUBLE_ELT, container.ptr, 2);
	} else {
		throw std::runtime_error("Unknown attribute type for container: " + container.name);
	}

	attrs.emplace_back(container.name, kind, type, container.ptr, -1);
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

void Model::setSelectedAttr(int idx, ColormapLayer layer) {
	// Under 0, no selection
	// TODO does it silent ?
	if (idx >= static_cast<int>(attrs.size()))
		return;

	selectedAttr[layer] = idx;
	unsetColormaps(layer);

	if (idx < 0) {
		return;
	}

	auto attrName = attrs[idx].name;
	ElementKind kind = attrs[idx].kind;

	setColormapAttr(attrName, kind, layer);
	setColormap(kind, layer);
}

void Model::resetLayer(ElementKind kind, Layer layer) {
	// Prepare a vector of zeros of the size of the element kind to unset
	std::vector<float> zeros;
	switch (kind) {
		case ElementKind::CELLS_ELT: {
			zeros.resize(ncells(), 0.f);
			break;
		}
		case ElementKind::CELL_FACETS_ELT:
		case ElementKind::FACETS_ELT: {
			zeros.resize(nfacets(), 0.f);
			break;
		}
		case ElementKind::EDGES_ELT: {
			zeros.resize(nhalfedges(), 0.f);
			break;
		}
		case ElementKind::CORNERS_ELT: 
		case ElementKind::CELL_CORNERS_ELT: {
			zeros.resize(ncorners(), 0.f);
			break;
		}
		case ElementKind::POINTS_ELT: {
			zeros.resize(nverts(), 0.f);
			break;
		}
	}

	// If renderer is rendering this kind of element, set requested layer data to zeros
	for (auto const &[k ,r] : _renderers) {
		if (r->isRenderElement(kind)) {
			r->setLayer(zeros, layer);
		}
	}
}

void Model::setLayer(ElementKind kind, Layer layer, bool update) {

	if (activatedLayers[{layer, kind}] && !update)
		return;

	for (auto const &[k, r] : _renderers) {
		if (r->isRenderElement(kind)) {
			r->setLayerElement(kind, layer);
		}
	}

	if (update)
		updateLayer(layer, kind);

	activatedLayers[{layer, kind}] = true;
}

void Model::unsetLayer(ElementKind kind, Layer layer, bool reset) {
	// Little optimisation, doesn't update data
	// if layer isn't activated, no need to unset
	if (!activatedLayers[{layer, kind}])
		return;
	
	// Set requested layer data to zeros
	if (reset)
		resetLayer(kind, layer);

	for (auto const &[k ,r] : _renderers) {
		if (r->isRenderElement(kind)) {
			r->setLayerElement(-1, layer); // element -1 means => deactivate layer
		}
	}

	activatedLayers[{layer, kind}] = false;
}