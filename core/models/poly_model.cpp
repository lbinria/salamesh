#include "poly_model.h"

bool PolyModel::load(const std::string path) {
	// TODO check if the model failed to read in ultimaille, else there is side effects ! 
	
	// Load the mesh
	_surfaceAttributes = read_by_extension(path, _m);
	_path = path;

	if (_m.nfacets() <= 0)
		return false;

	// All triangles / all quads ?
	bool allTri = true, allQuad = true;
	for (auto &f : _m.iter_facets()) {
		allTri &= f.size() == 3;
		allQuad &= f.size() == 4;
	}

	if (allTri || allQuad) {
		return false;
	}

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