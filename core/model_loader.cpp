#include "model_loader.h"

#include "scene.h"
#include "scene/scene_node.h"
#include "lines_geometry.h"

std::shared_ptr<SceneNode> ModelLoader::load(const std::string filename, const std::string name) {

	auto node = std::make_shared<SceneNode>(name);

	bool success = loadTriangles(filename, *node);
	
	if (!success)
		success = loadQuads(filename, *node);

	if (!success)
		success = loadPolygons(filename, *node);
	
	if (!success)
		success = loadPolyLine(filename, *node);

	// Put all compatible shaders on model
	for (auto &shader : _scene.getShaders()) {
		if (shader->isCompatible(node->getGeometry()))
			node->addShaderPass(*shader);
	}

	// Create BBox
	auto bbox = std::make_shared<SceneNode>(name + "_bbox");
	bbox->addShaderPass(_scene.getShader("line_shader").value());

	// auto bbox = std::make_shared<Lines>(name + "_bbox");
	// bbox->addMaterial()

	auto &bboxGeometry = bbox->createGeometry<LinesGeometry>();

	auto [min, max] = node->getGeometry().bbox();

	bboxGeometry.addLines({
		// Bottom face (z = min.z)
		{ vec3(min.x, min.y, min.z), vec3(max.x, min.y, min.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(max.x, min.y, min.z), vec3(max.x, max.y, min.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(max.x, max.y, min.z), vec3(min.x, max.y, min.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(min.x, max.y, min.z), vec3(min.x, min.y, min.z), vec3(1.0f, 1.0f, 1.0f) },

		// Top face (z = max.z)
		{ vec3(min.x, min.y, max.z), vec3(max.x, min.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(max.x, min.y, max.z), vec3(max.x, max.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(max.x, max.y, max.z), vec3(min.x, max.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(min.x, max.y, max.z), vec3(min.x, min.y, max.z), vec3(1.0f, 1.0f, 1.0f) },

		// Vertical edges
		{ vec3(min.x, min.y, min.z), vec3(min.x, min.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(max.x, min.y, min.z), vec3(max.x, min.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(max.x, max.y, min.z), vec3(max.x, max.y, max.z), vec3(1.0f, 1.0f, 1.0f) },
		{ vec3(min.x, max.y, min.z), vec3(min.x, max.y, max.z), vec3(1.0f, 1.0f, 1.0f) }
	});


	node->addChild(bbox);


	node->addViewComponent("points", node->getGeometry(), _scene.getShader("points_shader2").value());
	node->addViewComponent("mesh", node->getGeometry(), _scene.getShader("triangles_shader2").value());


	// Scene own mesh
	// auto &m = scene.getMesh("catorus");
	// m.get().vert(0) = {1,2,3};
	// m.requestUpdate();

	// auto &n = scene.getNode("catorus_0");
	// n.setPosition({1,0,0});
	// n.addViewComponent("point", m, scene.getShader("point_shader"));
	// n.addViewComponent("bbox", scene.getMesh("catorus_bbox"), scene.getShader("point_shader"));
	// n.components["point"]["light"]["enabled"] = false;
	// n.getMesh();

	// auto &n = scene.getNode("catorus_1");
	// n.setPosition({0,1,0});
	// n.addViewComponent("point", m, scene.getShader("point_shader"));
	// n.components["point"]["style"]["color"] = {1, 0, 0};

	// Node own mesh
	// auto &n = scene.getNode("catorus_0");
	// n.setPosition({1,0,0});
	// n.addViewComponent("point", n.getMesh("main"), scene.getShader("point_shader"));
	// n.addViewComponent("mesh", n.getMesh("main"), scene.getShader("tri_shader"));
	// n.addViewComponent("bbox", n.getMesh("bbox"), scene.getShader("line_shader"));
	// n.components["point"]["light"]["enabled"] = false;
	// n.getMesh();

	// n.add<PointComponent>("point")
	// n.add<TrianglesComponent>("mesh")
	// n.add<BBoxComponent>("bbox")


	return node;
}

bool ModelLoader::loadTriangles(const std::string &filename, SceneNode &node) {
	auto &geometry = node.createGeometry<TrianglesGeometry>();

	geometry._attributes = read_by_extension(filename, geometry._m);
	if (geometry._m.nfacets() <= 0)
		return false;

	geometry.path = filename;

	return true;
}

bool ModelLoader::loadQuads(const std::string &filename, SceneNode &node) {
	auto &geometry = node.createGeometry<QuadsGeometry>();

	geometry._attributes = read_by_extension(filename, geometry._m);
	if (geometry._m.nfacets() <= 0)
		return false;

	geometry.path = filename;

	return true;
}

bool ModelLoader::loadPolygons(const std::string &filename, SceneNode &node) {
	auto &geometry = node.createGeometry<PolygonsGeometry>();

	geometry._attributes = read_by_extension(filename, geometry._m);
	if (geometry._m.nfacets() <= 0)
		return false;

	geometry.path = filename;

	return true;
}

bool ModelLoader::loadPolyLine(const std::string &filename, SceneNode &node) {
	auto &geometry = node.createGeometry<PolyLineGeometry>();

	geometry._attributes = read_by_extension(filename, geometry._m);
	if (geometry._m.nedges() <= 0)
		return false;

	geometry.path = filename;

	return true;
}