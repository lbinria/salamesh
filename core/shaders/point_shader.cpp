#include "point_shader.h"
#include "opengl_helper.h"
#include "material_params.h"

#include "mesh_primitive.h"

bool PointShader::isCompatible(Mesh &mesh) {
	return true;
}

MeshBuffer PointShader::createMeshBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(PointVertex), (void*)offsetof(PointVertex, vertexIndex));
	sl::createVBOVec3(shader.id, "p", sizeof(PointVertex), (void*)offsetof(PointVertex, pos));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(PointVertex), (void*)offsetof(PointVertex, size));

	return MeshBuffer(vao, vbo);
};

Material PointShader::createMaterial() {
	return Material{{
		{"style", MaterialParams::getPointStyleMaterialParams()},
		{"clipping", MaterialParams::getClippingMaterialParams()},
		{"light", MaterialParams::getLightMaterialParams()}
	}};
}

void PointShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto stream = mesh.getPointsStream();
	meshBuffer.nelements = stream.size();
	meshBuffer.write(stream);
}
