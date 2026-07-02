#include "point_shader.h"
#include "opengl_helper.h"
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

GeometryBuffer PointShader::createGeometryBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOVec3(shader.id, "p", sizeof(Vertex), (void*)offsetof(Vertex, position));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(Vertex), (void*)offsetof(Vertex, size));

	return GeometryBuffer(vao, vbo);
};

Material PointShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<PointStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void PointShader::updatePointSet(GeometryBuffer &geometryBuffer, PointSet &ps) {
	std::vector<Vertex> vertices(ps.size());
	for (int i = 0; i < ps.size(); ++i) {
		auto &v = ps[i];

		vertices[i] = { 
			.vertexIndex = i,
			.position = sl::algebra::vecf(v),
			.size = 1.f
		};
	}

	geometryBuffer.nelements = vertices.size();
	glBindVertexArray(geometryBuffer.vao());
	glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer.vbo());
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}


void PointShader::update(GeometryBuffer &geometryBuffer, Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);
	if (trianglesGeometry)
		updatePointSet(geometryBuffer, trianglesGeometry->_m.points);
	else if (auto quadsGeometry = dynamic_cast<QuadsGeometry*>(&geometry))
		updatePointSet(geometryBuffer, quadsGeometry->_m.points);
	else if (auto polygonsGeometry = dynamic_cast<PolygonsGeometry*>(&geometry))
		updatePointSet(geometryBuffer, polygonsGeometry->_m.points);
	else if (auto polyLineGeometry = dynamic_cast<PolyLineGeometry*>(&geometry))
		updatePointSet(geometryBuffer, polyLineGeometry->_m.points);
}


void PointShader::clear() {
}

void PointShader::clean() {
	// Clean up
	shader.clean();
}