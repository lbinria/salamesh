#include "point_shader.h"
#include "opengl_helper.h"
#include "material_params.h"
#include "layer_params.h"
#include "point_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

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
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(PointPrimitive), (void*)offsetof(PointPrimitive, id));
	sl::createVBOVec3(shader.id, "p", sizeof(PointPrimitive), (void*)offsetof(PointPrimitive, pos));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(PointPrimitive), (void*)offsetof(PointPrimitive, size));

	return MeshBuffer(vao, vbo);
};

Material PointShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<PointStyleParams>();
	params["colormap"] = std::make_shared<ColormapLayerParams>();
	params["highlight"] = std::make_shared<HighlightLayerParams>();
	params["filter"] = std::make_shared<FilterLayerParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void PointShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto stream = mesh.getPointsStream();
	meshBuffer.nelements = stream.size();
	meshBuffer.write(stream);
}
