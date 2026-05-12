#include "line_renderer.h"
#include "../helpers.h"
#include "../../core/utils/opengl_helper.h"

void LineMaterial::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	shader.use();

	// VBO
	sl::createVBOVec3(shader.id, "p", sizeof(LineComponent), (void*)offsetof(LineComponent, p));
	sl::createVBOVec3(shader.id, "color", sizeof(LineComponent), (void*)offsetof(LineComponent, color));

	std::cout << "LineMaterial init" << std::endl;
}

void LineMaterial::push() {

	std::vector<LineComponent> lineComponents;

	// Map Line -> LineComponent for VBO
	for (auto &l : lines) {
		lineComponents.push_back({.p = l.a, .color = l.color});
		lineComponents.push_back({ .p = l.b, .color = l.color });
	}

	writeVBOBuffer(lineComponents, true);
}

void LineMaterial::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	setPosition(position);

	glDrawArrays(GL_LINES, 0, nelements);
}

void LineMaterial::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(LineComponent), nullptr, GL_DYNAMIC_DRAW);
	clearLines();
}

void LineMaterial::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shader.clean();
}