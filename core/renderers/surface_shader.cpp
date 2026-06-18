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

ShaderBuffer SurfaceShader::createShaderBuffer() {
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
	return ShaderBuffer(shader, vao, vbo, params);
};

Material SurfaceShader::createMaterial() {
	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<MeshStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return Material(params);
}

void SurfaceShader::update(ShaderBuffer &shaderBuffer, Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);

	if (trianglesGeometry) {

		auto &m = trianglesGeometry->_m;
		shaderBuffer.nelements = m.nfacets() * 3 /* 3 points per tri */;

		std::vector<Vertex> vertices(shaderBuffer.nelements);
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

		glBindVertexArray(shaderBuffer.vao());
		glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer.vbo());
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	}

}

void SurfaceShader::init() {

	_params["style"] = std::make_shared<MeshStyleParams>();
	_params["layers"] = std::make_shared<LayersParams>();
	_params["clipping"] = std::make_shared<ClippingParams>();
	_params["light"] = std::make_shared<LightParams>();
	
	for (auto &[k, p] : _params) {
		p->init();
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	sl::createVBOVec3(shader.id, "p0", sizeof(Vertex), (void*)offsetof(Vertex, p0));
	sl::createVBOVec3(shader.id, "p1", sizeof(Vertex), (void*)offsetof(Vertex, p1));
	sl::createVBOVec3(shader.id, "p2", sizeof(Vertex), (void*)offsetof(Vertex, p2));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));
	sl::createVBOInteger(shader.id, "localIndex", sizeof(Vertex), (void*)offsetof(Vertex, localIndex));
	sl::createVBOInteger(shader.id, "cornerIndex", sizeof(Vertex), (void*)offsetof(Vertex, cornerIndex));
	
}

void SurfaceShader::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	setPosition(position);
	
	for (auto &[paramsName, params] : _params)
		params->apply(shader);

	glDrawArrays(GL_TRIANGLES, 0, nelements);
}

void SurfaceShader::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
}

void SurfaceShader::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// TODO important clean params

	// glDeleteBuffers(1, &bufHighlight);
	// glDeleteTextures(1, &tboHighlight);
	// glDeleteBuffers(1, &bufFilter);
	// glDeleteTextures(1, &tboFilter);
	// glDeleteBuffers(1, &bufColormap0);
	// glDeleteTextures(1, &tboColormap0);
	// glDeleteBuffers(1, &bufColormap1);
	// glDeleteTextures(1, &tboColormap1);
	// glDeleteBuffers(1, &bufColormap2);
	// glDeleteTextures(1, &tboColormap2);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}