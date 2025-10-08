#include "line_renderer.h"
#include "helpers.h"

void LineRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	shader.use();

	// VBO
	GLuint pLoc = glGetAttribLocation(shader.id, "p");
	glEnableVertexAttribArray(pLoc);
	glVertexAttribPointer(pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(LineComponent), (void*)offsetof(LineComponent, p));

	GLuint colorLoc = glGetAttribLocation(shader.id, "color");
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(LineComponent), (void*)offsetof(LineComponent, color));

}

void LineRenderer::push() {

	std::vector<LineComponent> lineComponents;

	// Map Line -> LineComponent for VBO
	for (auto &l : lines) {
		lineComponents.push_back({ p: l.a, color: l.color });
		lineComponents.push_back({ p: l.b, color: l.color });
	}

	nverts = lineComponents.size();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(LineComponent), lineComponents.data(), GL_DYNAMIC_DRAW);
}

void LineRenderer::render(glm::vec3 &position) {

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

void LineRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shader.clean();
}