#include "point_shader.h"
#include "opengl_helper.h"
#include "material_params.h"
#include "layer_params.h"
#include "point_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

#include "mesh_primitives.h"

bool PointShader::isCompatible(Mesh &mesh) {
	auto trianglesMesh = dynamic_cast<TrianglesMesh*>(&mesh);
	auto quadsMesh = dynamic_cast<QuadsMesh*>(&mesh);
	auto polygonsMesh = dynamic_cast<PolygonsMesh*>(&mesh);
	auto polylineMesh = dynamic_cast<PolyLineMesh*>(&mesh);
	auto tetrahedrasMesh = dynamic_cast<TetrahedrasMesh*>(&mesh);
	return trianglesMesh || quadsMesh || polygonsMesh || polylineMesh || tetrahedrasMesh;
}

MeshBuffer PointShader::createMeshBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(PointPrimitive), (void*)offsetof(PointPrimitive, id));
	sl::createVBOVec3(shader.id, "p", sizeof(PointPrimitive), (void*)offsetof(PointPrimitive, pos));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(PointPrimitive), (void*)offsetof(PointPrimitive, size));

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

void PointShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto stream = mesh.getPointsStream();
	meshBuffer.nelements = stream.size();
	meshBuffer.write(stream);
}

void PointShader::transformStream(std::vector<PointPrimitive> points) {

}
