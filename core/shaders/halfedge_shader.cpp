#include "halfedge_shader.h"
#include "opengl_helper.h"
#include "helpers.h"
#include "edge_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

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
	sl::createVBOInteger(shader.id, "halfedgeIndex", sizeof(EdgePrimitive), (void*)offsetof(EdgePrimitive, id));
	sl::createVBOVec3(shader.id, "aP0", sizeof(EdgePrimitive), (void*)offsetof(EdgePrimitive, p0));
	sl::createVBOVec3(shader.id, "aP1", sizeof(EdgePrimitive), (void*)offsetof(EdgePrimitive, p1));
	sl::createVBOFloat(shader.id, "aSide", sizeof(EdgePrimitive), (void*)offsetof(EdgePrimitive, side));
	sl::createVBOFloat(shader.id, "aEnd", sizeof(EdgePrimitive), (void*)offsetof(EdgePrimitive, end));
	sl::createVBOVec3(shader.id, "bary", sizeof(EdgePrimitive), (void*)offsetof(EdgePrimitive, bary));

	return MeshBuffer(vao, vbo);
};

Material HalfedgeShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<EdgeStyleParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void HalfedgeShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto edgeStream = mesh.getEdgesStream();
	meshBuffer.nelements = edgeStream.size();
	meshBuffer.write(edgeStream);
}