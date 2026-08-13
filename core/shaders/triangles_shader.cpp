#include "triangles_shader.h"
#include "opengl_helper.h"

bool TrianglesShader::isCompatible(Mesh &mesh) {
	auto trianglesMesh = dynamic_cast<TrianglesMesh*>(&mesh);
	return trianglesMesh;
}

MeshBuffer TrianglesShader::createMeshBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOVec3(shader.id, "p0", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, p0));
	sl::createVBOVec3(shader.id, "p1", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, p1));
	sl::createVBOVec3(shader.id, "p2", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, p2));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, facetIndex));
	sl::createVBOInteger(shader.id, "localIndex", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, localIndex));
	sl::createVBOInteger(shader.id, "cornerIndex", sizeof(TriangleVertex), (void*)offsetof(TriangleVertex, cornerIndex));

	return MeshBuffer(vao, vbo, MeshBuffer::Stream::TRIANGLES_STREAM);
};

Material TrianglesShader::createMaterial() {
	return Material{{
		{"style", MaterialParams::getStyleMaterialParams()},
		{"clipping", MaterialParams::getClippingMaterialParams()},
		{"light", MaterialParams::getLightMaterialParams()}
	}};
}

void TrianglesShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto stream = mesh.getTrianglesStream();
	meshBuffer.nelements = stream.size();
	meshBuffer.write(stream);
}