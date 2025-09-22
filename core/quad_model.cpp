#include "quad_model.h"

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

void QuadModel::push() {
	_quadRenderer.push();
	_pointSetRenderer.push();
	// _halfedgeRenderer.push();

	if (colorMode == ColorMode::ATTRIBUTE) {
		updateAttr();
	}
}