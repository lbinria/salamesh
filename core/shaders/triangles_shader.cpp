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
	sl::createVBOVec3(shader.id, "p0", sizeof(Vertex), (void*)offsetof(Vertex, p0));
	sl::createVBOVec3(shader.id, "p1", sizeof(Vertex), (void*)offsetof(Vertex, p1));
	sl::createVBOVec3(shader.id, "p2", sizeof(Vertex), (void*)offsetof(Vertex, p2));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));
	sl::createVBOInteger(shader.id, "localIndex", sizeof(Vertex), (void*)offsetof(Vertex, localIndex));
	sl::createVBOInteger(shader.id, "cornerIndex", sizeof(Vertex), (void*)offsetof(Vertex, cornerIndex));

	return MeshBuffer(vao, vbo);
};

Material TrianglesShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void TrianglesShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto trianglesMesh = dynamic_cast<TrianglesMesh*>(&mesh);

	if (trianglesMesh) {

		auto &m = trianglesMesh->_m;
		meshBuffer.nelements = m.nfacets() * 3 /* 3 points per tri */;

		std::vector<Vertex> vertices(meshBuffer.nelements);
		for (auto &f : m.iter_facets()) {

			auto p0 = f.vertex(0).pos();
			auto p1 = f.vertex(1).pos();
			auto p2 = f.vertex(2).pos();

			for (int lv = 0; lv < 3; ++lv) {
				auto v = f.vertex(lv);
				auto p = v.pos();
				const int firstCornerIdx = f * 3;
				const int c = firstCornerIdx + lv;
				
				vertices[c] = { 
					.localIndex = lv,
					.cornerIndex = firstCornerIdx,
					.p0 = sl::algebra::vecf(p0),
					.p1 = sl::algebra::vecf(p1),
					.p2 = sl::algebra::vecf(p2),
					.facetIndex = f
				};
			}
		}

		glNamedBufferData(meshBuffer.vbo(), vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	}

}