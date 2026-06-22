#include "surface_shader.h"
#include "../../core/utils/opengl_helper.h"
#include "mesh_style_params.h"
#include "layer_params.h"
#include "light_params.h"
#include "clipping_params.h"

bool SurfaceShader::isCompatible(Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);
	return trianglesGeometry;
}

GeometryBuffer SurfaceShader::createShaderBuffer() {
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

	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return GeometryBuffer(shader, vao, vbo);
};

Material SurfaceShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void SurfaceShader::update(GeometryBuffer &geometryBuffer, Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);

	if (trianglesGeometry) {

		auto &m = trianglesGeometry->_m;
		geometryBuffer.nelements = m.nfacets() * 3 /* 3 points per tri */;

		std::vector<Vertex> vertices(geometryBuffer.nelements);
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
					.p0 = glm::vec3(p0.x, p0.y, p0.z),
					.p1 = glm::vec3(p1.x, p1.y, p1.z),
					.p2 = glm::vec3(p2.x, p2.y, p2.z),
					.facetIndex = f
				};
			}
		}

		glBindVertexArray(geometryBuffer.vao());
		glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer.vbo());
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	}

}


void SurfaceShader::clear() {

}

void SurfaceShader::clean() {
	// Clean
	shader.clean();
}