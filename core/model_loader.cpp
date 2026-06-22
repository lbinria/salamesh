#include "scene.h"
#include "model_loader.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

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
	auto bbox = std::make_shared<SceneNode>();
	bbox->addShaderPass(_scene.getShader("line_shader"));

	auto &bboxGeometry = bbox->createGeometry<LinesGeometry>();

	auto [min, max] = node->getGeometry().bbox();

	bboxGeometry.addLines({
		// Bottom face (z = min.z)
		{ glm::vec3(min.x, min.y, min.z), glm::vec3(max.x, min.y, min.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(max.x, min.y, min.z), glm::vec3(max.x, max.y, min.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(max.x, max.y, min.z), glm::vec3(min.x, max.y, min.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(min.x, max.y, min.z), glm::vec3(min.x, min.y, min.z), glm::vec3(1.0f, 1.0f, 1.0f) },

		// Top face (z = max.z)
		{ glm::vec3(min.x, min.y, max.z), glm::vec3(max.x, min.y, max.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(max.x, min.y, max.z), glm::vec3(max.x, max.y, max.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(max.x, max.y, max.z), glm::vec3(min.x, max.y, max.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(min.x, max.y, max.z), glm::vec3(min.x, min.y, max.z), glm::vec3(1.0f, 1.0f, 1.0f) },

		// Vertical edges
		{ glm::vec3(min.x, min.y, min.z), glm::vec3(min.x, min.y, max.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(max.x, min.y, min.z), glm::vec3(max.x, min.y, max.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(max.x, max.y, min.z), glm::vec3(max.x, max.y, max.z), glm::vec3(1.0f, 1.0f, 1.0f) },
		{ glm::vec3(min.x, max.y, min.z), glm::vec3(min.x, max.y, max.z), glm::vec3(1.0f, 1.0f, 1.0f) }
	});


	node->addChild(bbox);

	return node;
}