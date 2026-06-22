#include "poly_shader.h"
#include "../../core/utils/opengl_helper.h"
#include "mesh_style_params.h"
#include "layer_params.h"
#include "light_params.h"
#include "clipping_params.h"

GeometryBuffer PolyShader::createShaderBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	// Setup VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOInteger(shader.id, "localIndex", sizeof(Vertex), (void*)offsetof(Vertex, localIndex));
	sl::createVBOInteger(shader.id, "cornerIndex", sizeof(Vertex), (void*)offsetof(Vertex, cornerIndex));
	sl::createVBOInteger(shader.id, "cornerOff", sizeof(Vertex), (void*)offsetof(Vertex, cornerOff));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));

	sl::createVBOVec3(shader.id, "p", sizeof(Vertex), (void*)offsetof(Vertex, p));
	sl::createVBOVec3(shader.id, "p0", sizeof(Vertex), (void*)offsetof(Vertex, p0));
	sl::createVBOVec3(shader.id, "p1", sizeof(Vertex), (void*)offsetof(Vertex, p1));
	sl::createVBOVec3(shader.id, "p2", sizeof(Vertex), (void*)offsetof(Vertex, p2));
	sl::createVBOVec3(shader.id, "n", sizeof(Vertex), (void*)offsetof(Vertex, n));

	sl::createTBO(bufNVertsPerFacet, texNVertsPerFacet);

	auto geometryBuffer = GeometryBuffer(shader, vao, vbo);
	geometryBuffer.tbos.push_back({ 
		.name = "nvertsPerFacetBuf", 
		.texUnit = 8, 
		.tex = texNVertsPerFacet
	}); // Add TBO texNVertsPerFacet at the texture 8

	return geometryBuffer;
};

Material PolyShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

bool PolyShader::isCompatible(Geometry &geometry) {
	// Accept QuadsGeometry / PolyGeometry
	auto quadsGeometry = dynamic_cast<QuadsGeometry*>(&geometry);
	auto polygonsGeometry = dynamic_cast<PolygonsGeometry*>(&geometry);
	return quadsGeometry || polygonsGeometry;
}

void PolyShader::update(GeometryBuffer &geometryBuffer, Geometry &geometry) {
	auto quadsGeometry = dynamic_cast<QuadsGeometry*>(&geometry);
	auto polygonsGeometry = dynamic_cast<PolygonsGeometry*>(&geometry);

	if (quadsGeometry || polygonsGeometry) {

		Surface& m = quadsGeometry ? static_cast<Surface&>(quadsGeometry->_m) : polygonsGeometry->_m;

		// auto &m = quadsGeometry->_m;
		std::vector<float> nVertsPerFacet(m.nfacets());

		// Compute number of triangles needed to represent a facet
		int ntri = 0;
		for (auto &f : m.iter_facets()) {
			int nvertsFacet = f.size();
			ntri += nvertsFacet;
			nVertsPerFacet[f] = static_cast<float>(nvertsFacet);
		}
		geometryBuffer.nelements = 3 * ntri /* 3 points per tri, n tri per facet */;

		int cornerOff = 0;
		std::vector<Vertex> vertices;
		for (auto &f : m.iter_facets()) {

			// There is as much triangles as vertices in facet,
			int nv = f.size();
			const int ntri = nv;

			// Compute bary
			glm::vec3 bary{0.};
			for (int v = 0; v < nv; ++v) {
				auto pos = f.vertex(v).pos();
				bary += glm::vec3{pos.x, pos.y, pos.z};
			}
			bary /= nv;

			// Compute normal
			std::vector<vec3> pts(nv);
			for (int v = 0; v < nv; ++v) {
				pts[v] = f.vertex(v).pos();
			}
			vec3 n = geo::normal(pts.data(), nv);

			

			for (int t = 0; t < ntri; ++t) {

				const int lv = t;
				// Three points of current triangle
				vec3 verts[3] = {vec3(bary.x, bary.y, bary.z) , f.vertex(lv).pos(), f.vertex((lv + 1) % nv).pos()};

				// Compute first corner index of the triangle
				int firstCornerIdx = cornerOff + lv;

				for (int i = 0; i < 3; ++i) {

					// Retrieve vertex id (0 is always the barycenter => no vertex associated)
					int v = i == 0 ? -1 : f.vertex((lv + (i - 1)) % nv);

					auto p = verts[i];
					auto p1 = verts[1];
					auto p2 = verts[2];

					vertices.push_back({
						.vertexIndex = v, // useless i think
						.localIndex = i,
						// .cornerIndex = firstCornerIdx,
						.cornerIndex = lv,
						.cornerOff = cornerOff,
						.facetIndex = f,
						.p = glm::vec3(p.x, p.y, p.z),
						.p0 = bary,
						.p1 = glm::vec3(p1.x, p1.y, p1.z),
						.p2 = glm::vec3(p2.x, p2.y, p2.z),
						.n = glm::vec3(n.x, n.y, n.z)
					});
				}
			}

			cornerOff += f.size();
		}

		glBindVertexArray(geometryBuffer.vao());
		glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer.vbo());

		// Write VBO
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Write TBO buffer
		glBindBuffer(GL_TEXTURE_BUFFER, bufNVertsPerFacet);
		glBufferData(GL_TEXTURE_BUFFER, nVertsPerFacet.size() * sizeof(float), nVertsPerFacet.data(), GL_STATIC_DRAW);

	}

}


void PolyShader::clear() {

}

void PolyShader::clean() {


	glDeleteBuffers(1, &bufNVertsPerFacet);
	glDeleteTextures(1, &texNVertsPerFacet);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}