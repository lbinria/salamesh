#include "line_shader.h"
#include "helpers.h"
#include "opengl_helper.h"
#include "material_params.h"
#include "layer_params.h"
#include "point_style_params.h"
#include "light_params.h"
#include "clipping_params.h"
#include "lines_geometry.h"

bool LineShader::isCompatible(Geometry &geometry) {
	auto lineGeometry = dynamic_cast<LinesGeometry*>(&geometry);
	return lineGeometry;
}

GeometryBuffer LineShader::createGeometryBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOVec3(shader.id, "p", sizeof(LineComponent), (void*)offsetof(LineComponent, p));
	sl::createVBOVec3(shader.id, "color", sizeof(LineComponent), (void*)offsetof(LineComponent, color));

	return GeometryBuffer(vao, vbo);
};

Material LineShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	// params["style"] = std::make_shared<PointStyleParams>();
	// params["layers"] = std::make_shared<LayersParams>();
	// params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void LineShader::update(GeometryBuffer &geometryBuffer, Geometry &geometry) {

	auto linesGeometry = dynamic_cast<LinesGeometry*>(&geometry);

	if (linesGeometry) {
		std::vector<LineComponent> lineComponents;

		// Map Line -> LineComponent for VBO
		for (auto &l : linesGeometry->getLines()) {
			sl::algebra::vec3 la = sl::algebra::vecf(l.a);
			sl::algebra::vec3 lb = sl::algebra::vecf(l.b);
			sl::algebra::vec3 c = sl::algebra::vecf(l.color);
			lineComponents.push_back({.p = la, .color = c});
			lineComponents.push_back({ .p = lb, .color = c });
		}

		geometryBuffer.nelements = lineComponents.size();
		glBindVertexArray(geometryBuffer.vao());
		glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer.vbo());
		glBufferData(GL_ARRAY_BUFFER, lineComponents.size() * sizeof(LineComponent), lineComponents.data(), GL_STATIC_DRAW);
	}
}


void LineShader::clear() {
}

void LineShader::clean() {
	// Clean up
	shader.clean();
}