#include "model_loader.h"

#include "scene.h"
#include "scene/scene_model.h"
#include "lines_mesh.h"




std::shared_ptr<SceneModel> ModelLoader::load(const std::string filename, const std::string name) {

	std::shared_ptr<Mesh> mesh = loadMesh<PolygonsMesh>(filename);

	// In that case polygon is a Triangles or Quads
	if (mesh && mesh->isRegular() && (mesh->nfacets() == 3 || mesh->nfacets() == 4)) {
		mesh = nullptr;
	}

	if (!mesh)
		mesh = loadMesh<TrianglesMesh>(filename);

	if (!mesh)
		mesh = loadMesh<QuadsMesh>(filename);

	if (!mesh)
		mesh = loadMesh<PolyLineMesh>(filename);

	if (!mesh)
		return nullptr;

	// Create model & add to scene
	auto model = _scene.createModel(name, mesh);

	// Add all compatible shaders
	for (auto &[shaderName, shader] : _scene.getShaders()) {
		if (shader->isCompatible(*mesh)) {
			model->addShaderPass(*shader);
		}
	}

	return model;
}