#include "scene.h"
#include "model_loader.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

std::shared_ptr<SceneNode> ModelLoader::load(const std::string filename) {
	auto node = std::make_shared<SceneNode>();

	bool success = loadTriangles(filename, *node);
	
	if (!success)
		success = loadQuads(filename, *node);

	if (!success)
		success = loadPolygons(filename, *node);
	
	if (!success)
		success = loadPolyLine(filename, *node);

	// BBox
	auto bboxGeometry = std::make_unique<LinesGeometry>();

	auto [min, max] = node->getGeometry().bbox();

	bboxGeometry->addLines({
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

	auto bbox = std::make_shared<SceneNode>();
	bbox->addShader(_scene.getShader("line_shader"));
	bbox->setGeometry(std::move(bboxGeometry));

	node->addChild(bbox);

	return node;
}