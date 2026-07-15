#include "model_loader.h"

#include "scene.h"
#include "scene/scene_node.h"
#include "lines_mesh.h"

std::shared_ptr<SceneNode> ModelLoader::load(const std::string filename, const std::string name) {

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

	// Create node & add to scene
	auto node = _scene.createNode(name, mesh);

	node->addShaderPass(_scene.getShader("point_shader").value());
	node->addShaderPass(_scene.getShader(meshShaderName).value());
	node->addShaderPass(_scene.getShader("halfedge_shader").value());

	return node;
}