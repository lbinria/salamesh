#include "halfedge_shader.h"
#include "opengl_helper.h"
#include "helpers.h"
#include "layer_params.h"
#include "edge_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

bool HalfedgeShader::isCompatible(Mesh &mesh) {
	auto trianglesMesh = dynamic_cast<TrianglesMesh*>(&mesh);
	auto quadsMesh = dynamic_cast<QuadsMesh*>(&mesh);
	auto polygonsMesh = dynamic_cast<PolygonsMesh*>(&mesh);
	auto polylineMesh = dynamic_cast<PolyLineMesh*>(&mesh);
	return trianglesMesh || quadsMesh || polygonsMesh || polylineMesh;
}

MeshBuffer HalfedgeShader::createMeshBuffer() {
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

	return MeshBuffer(vao, vbo);
};

Material HalfedgeShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<EdgeStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void HalfedgeShader::updateHalfedges(MeshBuffer &meshBuffer, PolyLine &m) {
	std::vector<LineVert> vertices;
	// pre-allocate to speed-up
	vertices.reserve(m.nedges() * 6 /* 1 quad, 2 tri per quad, 3 points per tri */); 

	for (auto &e : m.iter_edges()) {

		vec3 b = vec3(e.from() + e.to()) * .5;
		sl::algebra::vec3 bary = sl::algebra::vecf(b);

		sl::algebra::vec3 gp0 = sl::algebra::vecf(e.from());
		sl::algebra::vec3 gp1 = sl::algebra::vecf(e.to());

		// build the 4 “corner” vertices
		LineVert lv0{e, gp0, gp1, -1.0f, 0.0f, bary};  // corner: start, left side
		LineVert lv1{e, gp0, gp1, +1.0f, 0.0f, bary};  // corner: start, right side
		LineVert lv2{e, gp0, gp1, -1.0f, 1.0f, bary};  // corner: end,   left side
		LineVert lv3{e, gp0, gp1, +1.0f, 1.0f, bary};  // corner: end,   right side

		vertices.push_back(lv0);
		vertices.push_back(lv1);
		vertices.push_back(lv2);
		
		vertices.push_back(lv2);
		vertices.push_back(lv3);
		vertices.push_back(lv1);

	}

	meshBuffer.nelements = vertices.size();
	glBindVertexArray(meshBuffer.vao());
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.vbo());
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
}

void HalfedgeShader::updateHalfedges(MeshBuffer &meshBuffer, Surface &m) {
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
		sl::algebra::vec3 bary = sl::algebra::vecf(b);

		for (int i = 0; i < facetSize; ++i) {
			
			auto v0 = f.vertex(i);
			auto v1 = f.vertex((i + 1) % facetSize);
			auto p0 = v0.pos();
			auto p1 = v1.pos();
			
			int halfedgeIdx = f * facetSize + i;

			sl::algebra::vec3 gp0 = sl::algebra::vecf(p0);
			sl::algebra::vec3 gp1 = sl::algebra::vecf(p1);

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

	meshBuffer.nelements = vertices.size();
	glBindVertexArray(meshBuffer.vao());
	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.vbo());
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
}


void HalfedgeShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {
	auto trianglesMesh = dynamic_cast<TrianglesMesh*>(&mesh);
	if (trianglesMesh)
		updateHalfedges(meshBuffer, trianglesMesh->_m);
	else if (auto quadsMesh = dynamic_cast<QuadsMesh*>(&mesh))
		updateHalfedges(meshBuffer, quadsMesh->_m);
	else if (auto polygonsMesh = dynamic_cast<PolygonsMesh*>(&mesh))
		updateHalfedges(meshBuffer, polygonsMesh->_m);
	else if (auto polyLineMesh = dynamic_cast<PolyLineMesh*>(&mesh))
		updateHalfedges(meshBuffer, polyLineMesh->_m);
}


void HalfedgeShader::clear() {

}

void HalfedgeShader::clean() {
	// Clean up

	// Clean shader
	shader.clean();
}