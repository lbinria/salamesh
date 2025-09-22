#include "surface_model.h"

bool SurfaceModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	// Load the mesh
	// VolumeAttributes attributes = read_by_extension(path, _hex);
	_surfaceAttributes = read_by_extension(path, *_surface);
	_path = path;

	if (_surface->nfacets() <= 0)
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

void SurfaceModel::saveAs(const std::string path) const {
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

	write_by_extension(path, *_surface, attributes);
}

void SurfaceModel::save() const {
	saveAs(_path);
}

void SurfaceModel::setSelectedAttr(int idx) {
	selectedAttr = idx;
	int kind = attrs[idx].kind;
	// TODO see condition here not very smart maybe abstract renderers ?
	if (kind == ElementKind::POINTS) {
		_pointSetRenderer.setAttribute(attrs[idx].ptr.get(), -1);
	} else 
		_surfaceRenderer->setAttribute(attrs[idx].ptr.get(), kind);
}

void SurfaceModel::setSelectedAttr(std::string name, ElementKind kind) {
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

void SurfaceModel::push() {
	_surfaceRenderer->push();
	_pointSetRenderer.push();
	// _halfedgeRenderer.push();

	if (colorMode == ColorMode::ATTRIBUTE) {
		updateAttr();
	}
}