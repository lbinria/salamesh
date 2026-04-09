#include "line_renderer.h"
#include "../helpers.h"
#include "../../core/graphic_api.h"



void LineRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(LineComponent), nullptr, GL_DYNAMIC_DRAW);
	clearLines();
}

// void LineRenderer::init() {

// 	glGenVertexArrays(1, &VAO);
// 	glGenBuffers(1, &VBO);

// 	glBindVertexArray(VAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, VBO);

// 	// VBO
// 	sl::createVBOVec3(shader.id, "p", sizeof(LineComponent), (void*)offsetof(LineComponent, p));
// 	sl::createVBOVec3(shader.id, "color", sizeof(LineComponent), (void*)offsetof(LineComponent, color));

// 	std::cout << "LineRenderer init" << std::endl;
// }

// void LineRenderer::push() {

// 	std::vector<LineComponent> lineComponents;

// 	// Map Line -> LineComponent for VBO
// 	for (auto &l : lines) {
// 		lineComponents.push_back({.p = l.a, .color = l.color});
// 		lineComponents.push_back({ .p = l.b, .color = l.color });
// 	}

// 	writeVBOBuffer(lineComponents, true);
// }

// void LineRenderer::render(RendererView &rv, glm::vec3 &position) {
// 	if (!rv.visible)
// 		return;

// 	glBindVertexArray(VAO);
	
// 	glm::mat4 model = glm::mat4(1.0f);
// 	model = glm::translate(model, position);
	
// 	rv.use(shader);
// 	shader.setMat4("model", model);

// 	glDrawArrays(GL_LINES, 0, nelements);
// }

// void LineRenderer::clean() {
// 	// Clean up
// 	glDeleteVertexArrays(1, &VAO);
// 	glDeleteBuffers(1, &VBO);
// 	shader.clean();
// }