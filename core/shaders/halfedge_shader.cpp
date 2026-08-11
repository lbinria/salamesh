#include "halfedge_shader.h"
#include "opengl_helper.h"
#include "helpers.h"

bool HalfedgeShader::isCompatible(Mesh &mesh) {
	auto trianglesMesh = dynamic_cast<TrianglesMesh*>(&mesh);
	auto quadsMesh = dynamic_cast<QuadsMesh*>(&mesh);
	auto polygonsMesh = dynamic_cast<PolygonsMesh*>(&mesh);
	auto polylineMesh = dynamic_cast<PolyLineMesh*>(&mesh);
	return trianglesMesh || quadsMesh || polygonsMesh || polylineMesh;
}

MeshBuffer HalfedgeShader::createMeshBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOInteger(shader.id, "halfedgeIndex", sizeof(EdgeVertex), (void*)offsetof(EdgeVertex, vertexIndex));
	sl::createVBOVec3(shader.id, "aP0", sizeof(EdgeVertex), (void*)offsetof(EdgeVertex, p0));
	sl::createVBOVec3(shader.id, "aP1", sizeof(EdgeVertex), (void*)offsetof(EdgeVertex, p1));
	sl::createVBOFloat(shader.id, "aSide", sizeof(EdgeVertex), (void*)offsetof(EdgeVertex, side));
	sl::createVBOFloat(shader.id, "aEnd", sizeof(EdgeVertex), (void*)offsetof(EdgeVertex, end));
	sl::createVBOVec3(shader.id, "bary", sizeof(EdgeVertex), (void*)offsetof(EdgeVertex, bary));

	return MeshBuffer(vao, vbo);
};

Material HalfedgeShader::createMaterial() {
	return Material{{
		{"style", MaterialParams::getEdgeStyleMaterialParams()},
		{"clipping", MaterialParams::getClippingMaterialParams()},
		{"light", MaterialParams::getLightMaterialParams()}
	}};
}

void HalfedgeShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto edgeStream = mesh.getEdgesStream();
	meshBuffer.nelements = edgeStream.size();
	meshBuffer.write(edgeStream);
}