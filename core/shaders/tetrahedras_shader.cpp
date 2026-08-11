#include "tetrahedras_shader.h"
#include "opengl_helper.h"

bool TetrahedrasShader::isCompatible(Mesh &mesh) {
	auto tetrahedrasMesh = dynamic_cast<TetrahedrasMesh*>(&mesh);
	return tetrahedrasMesh;
}

MeshBuffer TetrahedrasShader::createMeshBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	// sl::createVBOInteger(shader.id, "cornerIndex", sizeof(Vertex), (void*)offsetof(Vertex, cornerIndex));
	sl::createVBOVec3(shader.id, "p0", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, p0)); 
	sl::createVBOVec3(shader.id, "p1", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, p1)); 
	sl::createVBOVec3(shader.id, "p2", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, p2)); 
	sl::createVBOVec3(shader.id, "bary", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, bary)); 
	sl::createVBOInteger(shader.id, "localIndex", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, localIndex));
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, vertexIndex));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, facetIndex));
	sl::createVBOInteger(shader.id, "cellIndex", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, cellIndex));

	return MeshBuffer(vao, vbo);
};

Material TetrahedrasShader::createMaterial() {
	return Material{{
		{"style", MaterialParams::getStyleMaterialParams()},
		{"clipping", MaterialParams::getClippingMaterialParams()},
		{"light", MaterialParams::getLightMaterialParams()}
	}};
}

void TetrahedrasShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto stream = mesh.getTrianglesStream();
	meshBuffer.nelements = stream.size();
	meshBuffer.write(stream);
}