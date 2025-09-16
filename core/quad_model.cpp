#include "quad_model.h"
#include <json.hpp>

// TODO refactor, exactly the same as TriModel

std::string QuadModel::save_state() const {
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

void QuadModel::load_state(json model_state) {
	
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

bool QuadModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	// Load the mesh
	_surfaceAttributes = read_by_extension(path, _quad);
	_path = path;

	if (_quad.nfacets() <= 0)
		return false;

	// Extract name
	if (_name.empty()) {
		std::filesystem::path p(path);
		_name = p.stem().string();
	}

	clearAttrs();

	for (auto &a : _surfaceAttributes.points) {
		addAttr(ElementKind::POINTS, a);
	}
	for (auto &a : _surfaceAttributes.facets) {
		addAttr(ElementKind::FACETS, a);
	}
	for (auto &a : _surfaceAttributes.corners) {
		addAttr(ElementKind::CORNERS, a);
	}

	init();
	push();
	return true;
}

// TODO refactor, exactly the same as TriModel
void QuadModel::saveAs(const std::string path) const {
	// Save the mesh
	if (path.empty()) {
		std::cerr << "Error: No path specified for saving the mesh." << std::endl;
		return;
	}

	// TODO check path validity
	
	// Save attributes ! Convert back from salamesh attributes to NamedContainer vectors
	std::vector<NamedContainer> point_attrs;
	std::vector<NamedContainer> facet_attrs;
	std::vector<NamedContainer> corner_attrs;
	for (auto &a : attrs) {
		std::string name = a.name;
		ElementKind kind = a.kind;
		auto &container = a.ptr;

		if (kind == ElementKind::POINTS) {
			point_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::FACETS) {
			facet_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::CORNERS) {
			corner_attrs.push_back(NamedContainer(name, container));
		}
	}

	SurfaceAttributes attributes(
		point_attrs,
		facet_attrs,
		corner_attrs
	);

	write_by_extension(path, _quad, attributes);
}

// TODO refactor, exactly the same for all models
void QuadModel::save() const {
	saveAs(_path);
}

void QuadModel::setSelectedAttr(int idx) {

	// Check attrs size
	if (idx < 0 || idx >= attrs.size()) {
		throw std::runtime_error(
			"Selected attribute index out of bound: " + 
			std::to_string(idx) + ", model has " + 
			std::to_string(attrs.size()) + 
			" attributes."
		);
	}

	selectedAttr = idx;
	int kind = attrs[idx].kind;
	// TODO see condition here not very smart maybe abstract renderers ?
	if (kind == ElementKind::POINTS) {
		_pointSetRenderer.setAttribute(attrs[idx].ptr.get(), -1);
	} else 
		_quadRenderer.setAttribute(attrs[idx].ptr.get(), kind);
}

void QuadModel::setSelectedAttr(std::string name, ElementKind kind) {
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

void QuadModel::push() {
	_quadRenderer.push();
	_pointSetRenderer.push();
	// _halfedgeRenderer.push();

	if (colorMode == Model::ColorMode::ATTRIBUTE) {
		updateAttr();
	}
}