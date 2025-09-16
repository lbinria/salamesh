#include "hex_model.h"

std::string HexModel::save_state() const {
	json j;
	j["name"] = _name;
	j["path"] = _path;
	j["position"] = { position.x, position.y, position.z };
	j["color_mode"] = colorMode;
	j["is_light_enabled"] = isLightEnabled;
	j["is_light_follow_view"] = isLightFollowView;
	j["is_clipping"] = isClipping;
	j["clipping_plane_point"] = { clippingPlanePoint.x, clippingPlanePoint.y, clippingPlanePoint.z };
	j["clipping_plane_normal"] = { clippingPlaneNormal.x, clippingPlaneNormal.y, clippingPlaneNormal.z };
	j["invert_clipping"] = invertClipping;
	j["mesh_size"] = meshSize;
	j["mesh_shrink"] = meshShrink;
	j["frag_render_mode"] = fragRenderMode;
	j["selected_colormap"] = selectedColormap;
	j["visible"] = visible;
	return j.dump(4);
}

void HexModel::load_state(json model_state) {
	
	_name = model_state["name"].get<std::string>();
	_path = model_state["path"].get<std::string>();

	position = glm::vec3(
		model_state["position"][0].get<float>(),
		model_state["position"][1].get<float>(),
		model_state["position"][2].get<float>()
	);
	
	setColorMode((Model::ColorMode)model_state["color_mode"].get<int>());

	setLight(model_state["is_light_enabled"].get<bool>());
	setLightFollowView(model_state["is_light_follow_view"].get<bool>());
	setClipping(model_state["is_clipping"].get<bool>());

	setClippingPlanePoint(glm::vec3(
		model_state["clipping_plane_point"][0].get<float>(),
		model_state["clipping_plane_point"][1].get<float>(),
		model_state["clipping_plane_point"][2].get<float>()
	));

	setClippingPlaneNormal(glm::vec3(
		model_state["clipping_plane_normal"][0].get<float>(),
		model_state["clipping_plane_normal"][1].get<float>(),
		model_state["clipping_plane_normal"][2].get<float>()
	));

	setInvertClipping(model_state["invert_clipping"].get<bool>());

	setMeshSize(model_state["mesh_size"].get<float>());
	setMeshShrink(model_state["mesh_shrink"].get<float>());
	
	setSelectedColormap(model_state["selected_colormap"].get<int>());
	setVisible(model_state["visible"].get<bool>());

	load(_path);
}

bool HexModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	// Load the mesh
	// VolumeAttributes attributes = read_by_extension(path, _m);
	_volumeAttributes = read_by_extension(path, _m);
	_path = path;


	if (_m.ncells() <= 0)
		return false;

	// Extract name
	if (_name.empty()) {
		std::filesystem::path p(path);
		_name = p.stem().string();
	}

	clearAttrs();

	for (auto &a : _volumeAttributes.points) {
		addAttr(ElementKind::POINTS, a);
	}
	for (auto a : _volumeAttributes.cell_corners) {
		addAttr(ElementKind::CELL_CORNERS, a);
	}
	for (auto a : _volumeAttributes.cell_facets) {
		addAttr(ElementKind::CELL_FACETS, a);
	}
	for (auto a : _volumeAttributes.cells) {
		addAttr(ElementKind::CELLS, a);
	}

	init();
	push();
	return true;
}

void HexModel::saveAs(const std::string path) const {
	// Save the mesh
	if (path.empty()) {
		std::cerr << "Error: No path specified for saving the mesh." << std::endl;
		return;
	}

	// TODO check path validity
	
	// Save attributes ! Convert back from salamesh attributes to NamedContainer vectors
	std::vector<NamedContainer> point_attrs;
	std::vector<NamedContainer> cell_corner_attrs;
	std::vector<NamedContainer> cell_facet_attrs;
	std::vector<NamedContainer> cell_attrs;
	for (auto &a : attrs) {
		std::string name = a.name;
		ElementKind kind = a.kind;
		auto &container = a.ptr;

		if (kind == ElementKind::POINTS) {
			point_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::CELL_CORNERS) {
			cell_corner_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::CELL_FACETS) {
			cell_facet_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::CELLS) {
			cell_attrs.push_back(NamedContainer(name, container));
		}
	}

	VolumeAttributes attributes(
		point_attrs,
		cell_attrs,
		cell_facet_attrs,
		cell_corner_attrs
	);

	write_by_extension(path, _m, attributes);
}

void HexModel::save() const {
	saveAs(_path);
}

void HexModel::setSelectedAttr(int idx) {
	selectedAttr = idx;
	int kind = attrs[idx].kind;
	// TODO see condition here not very smart maybe abstract renderers ?
	if (kind == ElementKind::POINTS) {
		_pointSetRenderer.setAttribute(attrs[idx].ptr.get(), -1);
	} else 
		_hexRenderer.setAttribute(attrs[idx].ptr.get(), kind);
}

void HexModel::setSelectedAttr(std::string name, ElementKind kind) {
	// Search attribute by name
	for (int i = 0; i < attrs.size(); ++i) {
		const auto &attr = attrs[i];

		if (attr.getName() == name && attr.getKind() == kind) {
			setSelectedAttr(i);
			return;
		}
	}

	throw std::runtime_error("Attribute not found: " + name);
}

void HexModel::push() {
	_hexRenderer.push();
	_pointSetRenderer.push();
	_halfedgeRenderer.push();

	if (colorMode == Model::ColorMode::ATTRIBUTE) {
		updateAttr();
	}
}