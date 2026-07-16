#include "point_shader.h"
#include "opengl_helper.h"
#include "material_params.h"
#include "layer_params.h"
#include "point_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

bool PointShader::isCompatible(Mesh &mesh) {
	auto trianglesMesh = dynamic_cast<TrianglesMesh*>(&mesh);
	auto quadsMesh = dynamic_cast<QuadsMesh*>(&mesh);
	auto polygonsMesh = dynamic_cast<PolygonsMesh*>(&mesh);
	auto polylineMesh = dynamic_cast<PolyLineMesh*>(&mesh);
	return trianglesMesh || quadsMesh || polygonsMesh || polylineMesh;
}

MeshBuffer PointShader::createMeshBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOVec3(shader.id, "p", sizeof(Vertex), (void*)offsetof(Vertex, position));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(Vertex), (void*)offsetof(Vertex, size));

	return MeshBuffer(vao, vbo);
};

Material PointShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<PointStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void PointShader::updatePointSet(MeshBuffer &meshBuffer, PointSet &ps) {
	std::vector<Vertex> vertices(ps.size());
	for (int i = 0; i < ps.size(); ++i) {
		auto &v = ps[i];

		vertices[i] = { 
			.vertexIndex = i,
			.position = sl::algebra::vecf(v),
			.size = 1.f
		};
	}

	meshBuffer.nelements = vertices.size();
	glBindVertexArray(meshBuffer.vao());
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.vbo());
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}


void PointShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto trianglesMesh = dynamic_cast<TrianglesMesh*>(&mesh);
	if (trianglesMesh)
		updatePointSet(meshBuffer, trianglesMesh->_m.points);
	else if (auto quadsMesh = dynamic_cast<QuadsMesh*>(&mesh))
		updatePointSet(meshBuffer, quadsMesh->_m.points);
	else if (auto polygonsMesh = dynamic_cast<PolygonsMesh*>(&mesh))
		updatePointSet(meshBuffer, polygonsMesh->_m.points);
	else if (auto polyLineMesh = dynamic_cast<PolyLineMesh*>(&mesh))
		updatePointSet(meshBuffer, polyLineMesh->_m.points);
}


void PointShader::clear() {
}

void PointShader::clean() {
	// Clean up
	shader.clean();
}