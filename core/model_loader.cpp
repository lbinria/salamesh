#include "model_loader.h"

#include "scene.h"
#include "scene/scene_model.h"
#include "lines_mesh.h"

std::shared_ptr<SceneModel> ModelLoader::load(const std::string filename, const std::string name) {

	// TODO important refactor this :vomit: 
	std::string meshShaderName = "a_triangle_shader";

	std::shared_ptr<Mesh> mesh;

	if (!mesh) {
		mesh = loadMesh<PolygonsMesh>(filename);
		if (mesh->isRegular()) {
			mesh = nullptr;
		}

		meshShaderName = "a_polygon_shader";
	}

	if (!mesh) {
		mesh = loadMesh<TrianglesMesh>(filename);
		meshShaderName = "a_triangle_shader";
	}

	if (!mesh) {
		mesh = loadMesh<QuadsMesh>(filename);
		meshShaderName = "a_polygon_shader";
	}

	if (!mesh)
		mesh = loadMesh<PolyLineMesh>(filename);

	if (!mesh)
		return nullptr;

	// Create model & add to scene
	auto model = _scene.createModel(name, mesh);

	model->addShaderPass(_scene.getShader("point_shader").value());
	model->addShaderPass(_scene.getShader(meshShaderName).value());
	model->addShaderPass(_scene.getShader("halfedge_shader").value());

	return model;
}