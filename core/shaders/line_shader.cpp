#include "line_shader.h"
#include "helpers.h"
#include "opengl_helper.h"
#include "material_params.h"
#include "point_style_params.h"
#include "light_params.h"
#include "clipping_params.h"
#include "lines_mesh.h"

bool LineShader::isCompatible(Mesh &mesh) {
	auto lineMesh = dynamic_cast<LinesMesh*>(&mesh);
	return lineMesh;
}

MeshBuffer LineShader::createMeshBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOVec3(shader.id, "p", sizeof(LineComponent), (void*)offsetof(LineComponent, p));
	sl::createVBOVec3(shader.id, "color", sizeof(LineComponent), (void*)offsetof(LineComponent, color));

	return MeshBuffer(vao, vbo);
};

Material LineShader::createMaterial() {
	return Material{{
		{"light", MaterialParams::getLightMaterialParams()}
	}};
}

void LineShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {

	auto linesMesh = dynamic_cast<LinesMesh*>(&mesh);

	if (linesMesh) {
		std::vector<LineComponent> lineComponents;

		// Map Line -> LineComponent for VBO
		for (auto &l : linesMesh->getLines()) {
			sl::algebra::vec3 la = sl::algebra::vecf(l.a);
			sl::algebra::vec3 lb = sl::algebra::vecf(l.b);
			sl::algebra::vec3 c = sl::algebra::vecf(l.color);
			lineComponents.push_back({.p = la, .color = c});
			lineComponents.push_back({ .p = lb, .color = c });
		}

		meshBuffer.nelements = lineComponents.size();
		glNamedBufferData(meshBuffer.vbo(), lineComponents.size() * sizeof(LineComponent), lineComponents.data(), GL_STATIC_DRAW);

	}
}