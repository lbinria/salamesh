#include "bbox_material.h"
#include "../../core/utils/opengl_helper.h"
#include "../helpers.h"

// void BBoxMaterial::init() {

// 	glGenVertexArrays(1, &VAO);
// 	glGenBuffers(1, &VBO);

// 	glBindVertexArray(VAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, VBO);

// 	shader.use();

// 	// VBO
// 	sl::createVBOVec3(shader.id, "p", sizeof(Vertex), (void*)offsetof(Vertex, p));

// }

// void BBoxMaterial::push() {

	
// 	std::vector<Vertex> vertices;

// 	// Search bbox
// 	glm::vec3 pMin{FLT_MAX, FLT_MAX, FLT_MAX};
// 	glm::vec3 pMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};
// 	for (auto &p : ps) {
// 		glm::vec3 glmP = sl::um2glm(p);
// 		pMin = glm::min(pMin, glmP);
// 		pMax = glm::max(pMax, glmP);
// 	}

// 	// Create bbox lines
// 	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });
// 	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });

// 	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });
// 	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });

// 	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });
// 	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });

// 	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });
// 	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });

// 	// Frame
// 	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });
// 	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });

// 	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });
// 	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });

// 	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });
// 	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });

// 	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });
// 	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });

// 	// Frame
// 	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });
// 	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });

// 	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });
// 	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });

// 	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });
// 	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });

// 	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });
// 	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });

// 	// Frame
// 	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });
// 	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });

// 	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });
// 	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });

// 	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });
// 	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });

// 	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });
// 	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });

// 	writeVBOBuffer(vertices);
// }

void BBoxMaterial::clear() {
}

void BBoxMaterial::clean() {
	shader.clean();
}