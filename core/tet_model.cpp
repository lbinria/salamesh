#include "tet_model.h"

bool TetModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	// Load the mesh
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

void TetModel::saveAs(const std::string path) const {
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

void TetModel::setSelectedAttr(int idx) {
	selectedAttr = idx;
	int kind = attrs[idx].kind;
	// TODO see condition here not very smart maybe abstract renderers ?
	if (kind == ElementKind::POINTS) {
		_pointSetRenderer.setAttribute(attrs[idx].ptr.get(), -1);
	} else 
		_meshRenderer->setAttribute(attrs[idx].ptr.get(), kind);
}