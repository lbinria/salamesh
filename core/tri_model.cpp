#include "tri_model.h"

bool TriModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	std::cout << "read by extension..." << std::endl;

	// Load the mesh
	_surfaceAttributes = read_by_extension(path, _tri);
	_path = path;

	std::cout << "check facets..." << std::endl;

	if (_tri.nfacets() <= 0)
		return false;

	std::cout << "extract name: " << _name << std::endl;


	// Extract name
	if (_name.empty()) {
		std::filesystem::path p(path);
		_name = p.stem().string();
	}

	std::cout << "clear attr..." << std::endl;


	clearAttrs();

	std::cout << "add point attr..." << std::endl;

	for (auto &a : _surfaceAttributes.points) {
		addAttr(ElementKind::POINTS_ELT, a);
	}

	std::cout << "add facet attr..." << std::endl;

	for (auto &a : _surfaceAttributes.facets) {
		addAttr(ElementKind::FACETS, a);
	}

	std::cout << "add corner attr..." << std::endl;


	for (auto &a : _surfaceAttributes.corners) {
		addAttr(ElementKind::CORNERS, a);
	}

	std::cout << "init..." << std::endl;

	init();

	std::cout << "model initialized." << std::endl;
	
	push();

	std::cout << "model pushed." << std::endl;

	return true;
}

void TriModel::saveAs(const std::string path) const {
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

		if (kind == ElementKind::POINTS_ELT) {
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

	write_by_extension(path, _tri, attributes);
}