#include "halfedge_shader.h"
#include "opengl_helper.h"
#include "helpers.h"
#include "layer_params.h"
#include "edge_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

bool HalfedgeShader::isCompatible(Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);
	auto quadsGeometry = dynamic_cast<QuadsGeometry*>(&geometry);
	auto polygonsGeometry = dynamic_cast<PolygonsGeometry*>(&geometry);
	return trianglesGeometry || quadsGeometry || polygonsGeometry;
}

GeometryBuffer HalfedgeShader::createShaderBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOInteger(shader.id, "halfedgeIndex", sizeof(LineVert), (void*)offsetof(LineVert, halfedgeIndex));
	sl::createVBOVec3(shader.id, "aP0", sizeof(LineVert), (void*)offsetof(LineVert, P0));
	sl::createVBOVec3(shader.id, "aP1", sizeof(LineVert), (void*)offsetof(LineVert, P1));
	sl::createVBOFloat(shader.id, "aSide", sizeof(LineVert), (void*)offsetof(LineVert, side));
	sl::createVBOFloat(shader.id, "aEnd", sizeof(LineVert), (void*)offsetof(LineVert, end));
	sl::createVBOVec3(shader.id, "bary", sizeof(LineVert), (void*)offsetof(LineVert, bary));

	return GeometryBuffer(shader, vao, vbo);
};

Material HalfedgeShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<EdgeStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void HalfedgeShader::updateHalfedges(GeometryBuffer &geometryBuffer, Surface &m) {
	std::vector<LineVert> vertices;
	// pre-allocate to speed-up
	vertices.reserve(m.nfacets() * 4 /* reserve for 4 side facets */ * 6 /* 1 quad, 2 tri per quad, 3 points per tri */); 

	for (auto &f : m.iter_facets()) {
		int facetSize = m.facet_size(f);

		vec3 b;
		for (int lv = 0; lv < f.size(); ++lv) {
			b += f.vertex(lv).pos();
		}
		b /= f.size();
		glm::vec3 bary = sl::um2glm(b);

		for (int i = 0; i < facetSize; ++i) {
			
			auto v0 = f.vertex(i);
			auto v1 = f.vertex((i + 1) % facetSize);
			auto p0 = v0.pos();
			auto p1 = v1.pos();
			
			int halfedgeIdx = f * facetSize + i;

			const glm::vec3 gp0(p0.x, p0.y, p0.z);
			const glm::vec3 gp1(p1.x, p1.y, p1.z);

			// build the 4 “corner” vertices
			LineVert lv0{halfedgeIdx, gp0, gp1, -1.0f, 0.0f, bary};  // corner: start, left side
			LineVert lv1{halfedgeIdx, gp0, gp1, +1.0f, 0.0f, bary};  // corner: start, right side
			LineVert lv2{halfedgeIdx, gp0, gp1, -1.0f, 1.0f, bary};  // corner: end,   left side
			LineVert lv3{halfedgeIdx, gp0, gp1, +1.0f, 1.0f, bary};  // corner: end,   right side

			vertices.push_back(lv0);
			vertices.push_back(lv1);
			vertices.push_back(lv2);
			
			vertices.push_back(lv2);
			vertices.push_back(lv3);
			vertices.push_back(lv1);

		}
	}

	geometryBuffer.nelements = vertices.size();
	glBindVertexArray(geometryBuffer.vao());
	glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer.vbo());
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
}


void HalfedgeShader::update(GeometryBuffer &geometryBuffer, Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);
	if (trianglesGeometry)
		updateHalfedges(geometryBuffer, trianglesGeometry->_m);
	else if (auto quadsGeometry = dynamic_cast<QuadsGeometry*>(&geometry))
		updateHalfedges(geometryBuffer, quadsGeometry->_m);
	else if (auto polygonsGeometry = dynamic_cast<PolygonsGeometry*>(&geometry))
		updateHalfedges(geometryBuffer, polygonsGeometry->_m);
}


void HalfedgeShader::clear() {

}

void HalfedgeShader::clean() {
	// Clean up

	// Clean shader
	shader.clean();
}