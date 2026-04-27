#include "poly_renderer.h"
#include "../../core/graphic_api.h"

MaterialInstance PolyRenderer::getDefaultMaterial() {
	MaterialInstance mat;
	mat.addParam<LightParams>("light");
	mat.addParam<ClippingParams>("clipping");
	mat.addParam<MeshStyleParams>("style");
	mat.addParam<LayersParams>("layers");
	mat.addBuffers<LayerBufferGroup>("layers");
	return mat;
}

Renderer::GeometricData PolyRenderer::getData() {
	vertices.clear();

	nVertsPerFacet.clear();
	nVertsPerFacet.resize(_m.nfacets());

	// Compute number of triangles needed to represent a facet
	int ntri = 0;
	for (auto &f : _m.iter_facets()) {
		int nvertsFacet = f.size();
		ntri += nvertsFacet;
		nVertsPerFacet[f] = static_cast<float>(nvertsFacet);
	}
	nelements = 3 * ntri /* 3 points per tri, n tri per facet */;

	int cornerOff = 0;
	for (auto &f : _m.iter_facets()) {

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

	return Renderer::GeometricData{ 
		.vboBuffer = vertices.data(), 
		.texBuffers = {
			{
				"nvertsPerFacetBuf", 
				GeometricData::TextureBufferData{ 
					.data = nVertsPerFacet.data(), 
					.size = nVertsPerFacet.size() 
				}
			}
		} 
	};
}

void PolyRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
	nelements = 0;
}


std::vector<std::string> PolyRenderer::getBuffers() {
	return {"nvertsPerFacetBuf"};
}


std::vector<Renderer::RendererElementField> PolyRenderer::getElementFields() {
	return {
		{
			.name = "vertexIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, vertexIndex))
		},
		{
			.name = "localIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, localIndex))
		},
		{
			.name = "cornerIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, cornerIndex))
		},
		{
			.name = "cornerOff",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, cornerOff))
		},
		{
			.name = "facetIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, facetIndex))
		},
		{
			.name = "p",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, p))
		},
		{
			.name = "p0",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, p0))
		},
		{
			.name = "p1",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, p1))
		},
		{
			.name = "p2",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, p2))
		},
		{
			.name = "n",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, n))
		}
	};
}