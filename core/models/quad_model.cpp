#include "quad_model.h"

bool QuadModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	// Load the mesh
	_surfaceAttributes = read_by_extension(path, _m);
	_path = path;

	if (_m.nfacets() <= 0)
		return false;

	// Extract name
	if (_name.empty()) {
		std::filesystem::path p(path);
		_name = p.stem().string();
	}

	clearAttrs();

	auto containers = getAttributeContainers();
	for (auto &[k, c] : containers) {
		addAttr(k, c);
	}

	init();
	push();

	return true;
}

// TODO refactor, exactly the same as TriModel
void QuadModel::saveAs(const std::string path) const {
	// Check path validity
	if (path.empty()) {
		std::cerr << "Error: No path specified for saving the mesh." << std::endl;
		return;
	}

	// Save attributes ! Convert back from salamesh attributes to NamedContainer vectors
	std::vector<NamedContainer> point_attrs;
	std::vector<NamedContainer> facet_attrs;
	std::vector<NamedContainer> corner_attrs;
	for (auto &a : attrs) {
		std::string name = a.name;
		ElementKind kind = a.kind;
		auto &container = a.ptr;

		if (kind == ElementKind::POINTS_ELT) {
			point_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::FACETS_ELT) {
			facet_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::CORNERS_ELT) {
			corner_attrs.push_back(NamedContainer(name, container));
		}
	}

	SurfaceAttributes attributes(
		point_attrs,
		facet_attrs,
		corner_attrs
	);

	write_by_extension(path, _m, attributes);
}