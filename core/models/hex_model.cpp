#include "hex_model.h"

bool HexModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	// Load the mesh

	// Remove cout
	_volumeAttributes = read_by_extension(path, _m);
	_path = path;

	if (_m.ncells() <= 0)
		return false;

	clearAttrs();

	auto containers = getAttributeContainers();
	for (auto &[k, c] : containers) {
		addAttr(k, c);
	}

	init();
	push();
	
	return true;
}

bool HexModel::saveAs(const std::string path, std::vector<Attribute> attrs) const {
	// Check path validity
	if (path.empty()) {
		std::cerr << "Error: No path specified for saving the mesh." << std::endl;
		return false;
	}
	
	// Save attributes ! Convert back from salamesh attributes to NamedContainer vectors
	std::vector<NamedContainer> point_attrs;
	std::vector<NamedContainer> cell_corner_attrs;
	std::vector<NamedContainer> cell_facet_attrs;
	std::vector<NamedContainer> cell_attrs;
	for (auto &a : attrs) {
		std::string name = a.name;
		ElementKind kind = a.kind;
		auto &container = a.ptr;

		if (kind == ElementKind::POINTS_ELT) {
			point_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::CELL_CORNERS_ELT) {
			cell_corner_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::CELL_FACETS_ELT) {
			cell_facet_attrs.push_back(NamedContainer(name, container));
		} else if (kind == ElementKind::CELLS_ELT) {
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

	return true;
}