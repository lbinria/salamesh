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
	for (auto &[_, shader] : _scene.getShaders()) {
		if (shader->isCompatible(node->getGeometry()))
			node->addShaderPass(*shader);
	}

	// Create BBox
	auto bbox = std::make_shared<SceneNode>(name + "_bbox");
	bbox->addShaderPass(_scene.getShader("line_shader"));

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