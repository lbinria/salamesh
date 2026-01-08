#include "surface_model.h"

void SurfaceModel::saveAs(const std::string path) const {
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
		// TODO do something more clear here !
		// Do not save attributes 
		// For example, attr : vec2 => attr[0], attr[1] aren't saved
		if (a.selectedDim != -1)
			continue;

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

	write_by_extension(path, getSurface(), attributes);
}