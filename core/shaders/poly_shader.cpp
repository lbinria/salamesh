#include "poly_shader.h"
#include "opengl_helper.h"
#include "mesh_style_params.h"
#include "layer_params.h"
#include "light_params.h"
#include "clipping_params.h"

MeshBuffer PolyShader::createMeshBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	// Setup VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, id));
	sl::createVBOInteger(shader.id, "localIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, localIndex));
	sl::createVBOInteger(shader.id, "cornerIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, cornerIndex));
	sl::createVBOInteger(shader.id, "cornerOff", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, cornerOff));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, facetIndex));

	sl::createVBOVec3(shader.id, "p", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p));
	sl::createVBOVec3(shader.id, "p0", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p0));
	sl::createVBOVec3(shader.id, "p1", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p1));
	sl::createVBOVec3(shader.id, "p2", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, p2));
	sl::createVBOVec3(shader.id, "n", sizeof(TrianglePrimitive), (void*)offsetof(TrianglePrimitive, normal));

	unsigned int bufNVertsPerFacet, texNVertsPerFacet;
	sl::createTBO(bufNVertsPerFacet, texNVertsPerFacet);

	auto meshBuffer = MeshBuffer(vao, vbo);
	meshBuffer.tbos["nvertsPerFacetBuf"] = { 
		.name = "nvertsPerFacetBuf", 
		.texUnit = 8, 
		.tex = texNVertsPerFacet,
		.buf = bufNVertsPerFacet
	}; // Add TBO texNVertsPerFacet at the texture 8


	return meshBuffer;
};

Material PolyShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["colormap"] = std::make_shared<ColormapLayerParams>();
	params["highlight"] = std::make_shared<HighlightLayerParams>();
	params["filter"] = std::make_shared<FilterLayerParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

bool PolyShader::isCompatible(Mesh &mesh) {
	// Accept QuadsMesh / PolyMesh
	auto quadsMesh = dynamic_cast<QuadsMesh*>(&mesh);
	auto polygonsMesh = dynamic_cast<PolygonsMesh*>(&mesh);
	return quadsMesh || polygonsMesh;
}

void PolyShader::update(MeshBuffer &meshBuffer, Mesh &mesh) {

		std::vector<float> nVertsPerFacet(mesh.nfacets());

		// Compute number of triangles needed to represent a facet
		for (int fi = 0; fi < mesh.nfacets(); ++fi) {
			int nvertsFacet = mesh.facetSize(fi);
			nVertsPerFacet[fi] = static_cast<float>(nvertsFacet);
		}

		auto stream = mesh.getTrianglesStream();
		meshBuffer.nelements = stream.size();
		// Write VBO buffer
		meshBuffer.write(stream);
		// Write TBO buffer
		meshBuffer.write("nvertsPerFacetBuf", nVertsPerFacet);
}