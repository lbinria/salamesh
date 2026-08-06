#include "tetrahedras_shader.h"
#include "opengl_helper.h"
#include "mesh_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

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
	sl::createVBOVec3(shader.id, "p0", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p0)); 
	sl::createVBOVec3(shader.id, "p1", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p1)); 
	sl::createVBOVec3(shader.id, "p2", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p2)); 
	sl::createVBOVec3(shader.id, "bary", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, bary)); 
	sl::createVBOInteger(shader.id, "localIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, localIndex));
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, id));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, facetIndex));
	sl::createVBOInteger(shader.id, "cellIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, cellIndex));

	return MeshBuffer(vao, vbo);
};

Material TetrahedrasShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void TetrahedrasShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto stream = mesh.getTrianglesStream();
	meshBuffer.nelements = stream.size();
	meshBuffer.write(stream);
}