#include "tetrahedras_shader.h"
#include "opengl_helper.h"
#include "mesh_style_params.h"
#include "layer_params.h"
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
	sl::createVBOVec3(shader.id, "p0", sizeof(Vertex), (void*)offsetof(Vertex, p0)); 
	sl::createVBOVec3(shader.id, "p1", sizeof(Vertex), (void*)offsetof(Vertex, p1)); 
	sl::createVBOVec3(shader.id, "p2", sizeof(Vertex), (void*)offsetof(Vertex, p2)); 
	sl::createVBOVec3(shader.id, "bary", sizeof(Vertex), (void*)offsetof(Vertex, bary)); 
	sl::createVBOInteger(shader.id, "localIndex", sizeof(Vertex), (void*)offsetof(Vertex, localIndex));
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));
	sl::createVBOInteger(shader.id, "cellIndex", sizeof(Vertex), (void*)offsetof(Vertex, cellIndex));

	return MeshBuffer(vao, vbo);
};

Material TetrahedrasShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void TetrahedrasShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto tetrahedrasMesh = dynamic_cast<TetrahedrasMesh*>(&mesh);

	if (tetrahedrasMesh) {

		auto &m = tetrahedrasMesh->_m;
		meshBuffer.nelements = m.ncells() * 4 /* 4 facets per cell */ * 3 /* 3 points per facet */;

		// std::vector<Vertex> vertices(meshBuffer.nelements);
		std::vector<Vertex> vertices;
		vertices.reserve(meshBuffer.nelements);

		for (auto &c : m.iter_cells()) {

			vec3 v0 = c.vertex(0);
			vec3 v1 = c.vertex(1);
			vec3 v2 = c.vertex(2);
			vec3 v3 = c.vertex(3);

			vec3 b = (v0 + v1 + v2 + v3) / 4.;

			for (auto &f : c.iter_facets()) {
				
				vec3 p0 = f.vertex(0);
				vec3 p1 = f.vertex(1);
				vec3 p2 = f.vertex(2);

				for (int lv = 0; lv < 3; ++lv) {
					
					vertices.push_back({
						.p0 = sl::algebra::vecf(p0),
						.p1 = sl::algebra::vecf(p1),
						.p2 = sl::algebra::vecf(p2),
						.bary = sl::algebra::vecf(b),
						.localIndex = lv,
						.vertexIndex = f.vertex(lv),
						.facetIndex = f,
						.cellIndex = c
					});
				}
			}
		}

		glNamedBufferData(meshBuffer.vbo(), vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	}

}


void TetrahedrasShader::clear() {

}

void TetrahedrasShader::clean() {
	// Clean
	shader.clean();
}