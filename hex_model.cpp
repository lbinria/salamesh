#include "hex_model.h"

void HexModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	// Load the mesh
	// VolumeAttributes attributes = read_by_extension(path, _hex);
	_volumeAttributes = read_by_extension(path, _hex);
	_path = path;
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
}

void HexModel::save_as(const std::string path) const {
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

	write_by_extension(path, _hex, attributes);
}

void HexModel::save() const {
	save_as(_path);
}

void HexModel::setSelectedAttr(int idx) {
	selectedAttr = idx;
	int kind = attrs[idx].kind;
	// TODO see condition here not very smart maybe abstract renderers ?
	if (kind == ElementKind::POINTS) {
		_pointSetRenderer.setAttribute(attrs[idx].ptr.get());
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

	if (colorMode == Model::ColorMode::ATTRIBUTE) {
		updateAttr();
	}
}