#include "model_loader.h"

#include "scene.h"
#include "scene/scene_node.h"
#include "lines_geometry.h"

std::shared_ptr<SceneNode> ModelLoader::load(const std::string filename, const std::string name) {

	// TODO important refactor this :vomit: 
	std::string meshShaderName = "triangles_shader2";

	std::shared_ptr<Geometry> geometry;
	geometry = loadGeometry<TrianglesGeometry>(filename);
	
	if (!geometry) {
		geometry = loadGeometry<QuadsGeometry>(filename);
		meshShaderName = "polygons_shader2";
	}

	if (!geometry) {
		geometry = loadGeometry<PolygonsGeometry>(filename);
		meshShaderName = "polygons_shader2";
	}
	
	if (!geometry)
		geometry = loadGeometry<PolyLineGeometry>(filename);

	// Create node & add to scene
	auto node = _scene.createNode(name, geometry);

	node->addShaderPass("points", _scene.getShader("points_shader2").value());
	node->addShaderPass("mesh", _scene.getShader(meshShaderName).value());
	node->addShaderPass("halfedges", _scene.getShader("halfedges_shader2").value());

	return node;
}


// 	auto [min, max] = node->getGeometry().bbox();

// 	bboxGeometry.addLines({
// 		// Bottom face (z = min.z)
// 		{ vec3(min.x, min.y, min.z), vec3(max.x, min.y, min.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(max.x, min.y, min.z), vec3(max.x, max.y, min.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(max.x, max.y, min.z), vec3(min.x, max.y, min.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(min.x, max.y, min.z), vec3(min.x, min.y, min.z), vec3(1.0f, 1.0f, 1.0f) },

// 		// Top face (z = max.z)
// 		{ vec3(min.x, min.y, max.z), vec3(max.x, min.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(max.x, min.y, max.z), vec3(max.x, max.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(max.x, max.y, max.z), vec3(min.x, max.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(min.x, max.y, max.z), vec3(min.x, min.y, max.z), vec3(1.0f, 1.0f, 1.0f) },

// 		// Vertical edges
// 		{ vec3(min.x, min.y, min.z), vec3(min.x, min.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(max.x, min.y, min.z), vec3(max.x, min.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(max.x, max.y, min.z), vec3(max.x, max.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
// 		{ vec3(min.x, max.y, min.z), vec3(min.x, max.y, max.z), vec3(1.0f, 1.0f, 1.0f) }
// 	});

// 	// Scene own mesh
// 	// auto &m = scene.getMesh("catorus");
// 	// m.get().vert(0) = {1,2,3};
// 	// m.requestUpdate();

// 	// auto &n = scene.getNode("catorus_0");
// 	// n.setPosition({1,0,0});
// 	// n.addViewComponent("point", m, scene.getShader("point_shader"));
// 	// n.addViewComponent("bbox", scene.getMesh("catorus_bbox"), scene.getShader("point_shader"));
// 	// n.components["point"]["light"]["enabled"] = false;
// 	// n.getMesh();

// 	// auto &n = scene.getNode("catorus_1");
// 	// n.setPosition({0,1,0});
// 	// n.addViewComponent("point", m, scene.getShader("point_shader"));
// 	// n.components["point"]["style"]["color"] = {1, 0, 0};

// 	// Node own mesh
// 	// auto &n = scene.getNode("catorus_0");
// 	// n.setPosition({1,0,0});
// 	// n.addViewComponent("point", n.getMesh("main"), scene.getShader("point_shader"));
// 	// n.addViewComponent("mesh", n.getMesh("main"), scene.getShader("tri_shader"));
// 	// n.addViewComponent("bbox", n.getMesh("bbox"), scene.getShader("line_shader"));
// 	// n.components["point"]["light"]["enabled"] = false;
// 	// n.getMesh();

// 	// n.add<PointComponent>("point")
// 	// n.add<TrianglesComponent>("mesh")
// 	// n.add<BBoxComponent>("bbox")


// 	return node;
// }