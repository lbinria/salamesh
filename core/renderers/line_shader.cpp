#include "line_shader.h"
#include "../helpers.h"
#include "../../core/utils/opengl_helper.h"
#include "material_params.h"
#include "layer_params.h"
#include "point_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

bool LineShader::isCompatible(Geometry &geometry) {
	auto lineGeometry = dynamic_cast<LinesGeometry*>(&geometry);
	return lineGeometry;
}

ShaderBuffer LineShader::createShaderBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOVec3(shader.id, "p", sizeof(LineComponent), (void*)offsetof(LineComponent, p));
	sl::createVBOVec3(shader.id, "color", sizeof(LineComponent), (void*)offsetof(LineComponent, color));

	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	// params["style"] = std::make_shared<PointStyleParams>();
	// params["layers"] = std::make_shared<LayersParams>();
	// params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return ShaderBuffer(shader, vao, vbo, params);
};

Material LineShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<PointStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void LineShader::update(ShaderBuffer &shaderBuffer, Geometry &geometry) {

	auto linesGeometry = dynamic_cast<LinesGeometry*>(&geometry);

	if (linesGeometry) {
		std::vector<LineComponent> lineComponents;

		// Map Line -> LineComponent for VBO
		for (auto &l : linesGeometry->getLines()) {
			lineComponents.push_back({.p = l.a, .color = l.color});
			lineComponents.push_back({ .p = l.b, .color = l.color });
		}

		shaderBuffer.nelements = lineComponents.size();
		glBindVertexArray(shaderBuffer.vao());
		glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer.vbo());
		glBufferData(GL_ARRAY_BUFFER, lineComponents.size() * sizeof(LineComponent), lineComponents.data(), GL_STATIC_DRAW);
	}
}

void LineShader::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	shader.use();

	// VBO
	sl::createVBOVec3(shader.id, "p", sizeof(LineComponent), (void*)offsetof(LineComponent, p));
	sl::createVBOVec3(shader.id, "color", sizeof(LineComponent), (void*)offsetof(LineComponent, color));

	std::cout << "LineShader init" << std::endl;
}

void LineShader::push() {

	std::vector<LineComponent> lineComponents;

	// Map Line -> LineComponent for VBO
	for (auto &l : lines) {
		lineComponents.push_back({.p = l.a, .color = l.color});
		lineComponents.push_back({ .p = l.b, .color = l.color });
	}

	writeVBOBuffer(lineComponents, true);
}

void LineShader::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	setPosition(position);

	glDrawArrays(GL_LINES, 0, nelements);
}

void LineShader::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(LineComponent), nullptr, GL_DYNAMIC_DRAW);
	clearLines();
}

void LineShader::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shader.clean();
}