#include "triangles_shader.h"
#include "opengl_helper.h"
#include "mesh_style_params.h"
#include "layer_params.h"
#include "light_params.h"
#include "clipping_params.h"

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
	sl::createVBOVec3(shader.id, "p0", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p0));
	sl::createVBOVec3(shader.id, "p1", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p1));
	sl::createVBOVec3(shader.id, "p2", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p2));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, facetIndex));
	sl::createVBOInteger(shader.id, "localIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, localIndex));
	sl::createVBOInteger(shader.id, "cornerIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, cornerIndex));

	return MeshBuffer(vao, vbo);
};

Material TrianglesShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["colormap"] = std::make_shared<ColormapLayerParams>();
	params["highlight"] = std::make_shared<HighlightLayerParams>();
	params["filter"] = std::make_shared<FilterLayerParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void TrianglesShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto stream = mesh.getTrianglesStream();
	meshBuffer.nelements = stream.size();
	meshBuffer.write(stream);
}