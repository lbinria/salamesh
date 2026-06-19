#include "point_shader.h"
#include "../../core/utils/opengl_helper.h"
#include "material_params.h"
#include "layer_params.h"
#include "point_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

bool PointShader::isCompatible(Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);
	auto quadsGeometry = dynamic_cast<QuadsGeometry*>(&geometry);
	auto polygonsGeometry = dynamic_cast<PolygonsGeometry*>(&geometry);
	return trianglesGeometry || quadsGeometry || polygonsGeometry;
}

ShaderBuffer PointShader::createShaderBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOVec3(shader.id, "aPos", sizeof(Vertex), (void*)offsetof(Vertex, position));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(Vertex), (void*)offsetof(Vertex, size));

	return ShaderBuffer(shader, vao, vbo);
};

Material PointShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<PointStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void PointShader::updatePointSet(ShaderBuffer &shaderBuffer, PointSet &ps) {
	std::vector<Vertex> vertices(ps.size());
	for (int i = 0; i < ps.size(); ++i) {
		auto &v = ps[i];

		vertices[i] = { 
			.vertexIndex = i,
			.position = glm::vec3(v.x, v.y, v.z),
			.size = 1.f
		};
	}

	shaderBuffer.nelements = vertices.size();
	glBindVertexArray(shaderBuffer.vao());
	glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer.vbo());
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}


void PointShader::update(ShaderBuffer &shaderBuffer, Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);
	if (trianglesGeometry)
		updatePointSet(shaderBuffer, trianglesGeometry->_m.points);
	else if (auto quadsGeometry = dynamic_cast<QuadsGeometry*>(&geometry))
		updatePointSet(shaderBuffer, quadsGeometry->_m.points);
	else if (auto polygonsGeometry = dynamic_cast<PolygonsGeometry*>(&geometry))
		updatePointSet(shaderBuffer, polygonsGeometry->_m.points);
}


void PointShader::clear() {
}

void PointShader::clean() {
	// Clean up
	shader.clean();
}