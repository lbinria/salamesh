#include "bbox_renderer.h"
#include "helpers.h"

void BBoxRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	shader.use();

	// VBO
	GLuint pLoc = glGetAttribLocation(shader.id, "p");
	glEnableVertexAttribArray(pLoc);
	glVertexAttribPointer(pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p));

}

void BBoxRenderer::push() {

	
	std::vector<Vertex> vertices;

	// Search bbox
	glm::vec3 pMin{FLT_MAX, FLT_MAX, FLT_MAX};
	glm::vec3 pMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};
	for (auto &p : ps) {
		glm::vec3 glmP = sl::um2glm(p);
		pMin = glm::min(pMin, glmP);
		pMax = glm::max(pMax, glmP);
	}

	// Create bbox lines
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });

	// Frame
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });

	// Frame
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMin.x, pMax.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMin.x, pMin.y, pMin.z) });

	// Frame
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMax.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });

	vertices.push_back({ glm::vec3(pMax.x, pMax.y, pMin.z) });
	vertices.push_back({ glm::vec3(pMax.x, pMin.y, pMin.z) });

	nverts = vertices.size();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void BBoxRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	// Draw	
    shader.use();
	shader.setMat4("model", model);

	glDrawArrays(GL_LINES, 0, nverts);
}

void BBoxRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// TODO clean buffers, textures, unmap ptr...

	shader.clean();
}