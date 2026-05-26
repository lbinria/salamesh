#include "point_set_renderer.h"
#include "../../core/utils/opengl_helper.h"
#include "material_params.h"
#include "layer_params.h"
#include "point_style_params.h"
#include "light_params.h"
#include "clipping_params.h"

// Node = { Mesh, ShaderBuffers }
// Node::addShader(PointShader::createBuffer())
// ShaderStructure::createBuffer() => ShaderBuffer{vao, vbo, shader, ShaderParams }

bool PointMaterial::isCompatible(Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);
	auto quadsGeometry = dynamic_cast<QuadsGeometry*>(&geometry);
	return trianglesGeometry || quadsGeometry;
}

ShaderBuffer PointMaterial::createShaderBuffer() {
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// setup VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOVec3(shader.id, "aPos", sizeof(Vertex), (void*)offsetof(Vertex, position));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(Vertex), (void*)offsetof(Vertex, size));

	std::map<std::string, std::shared_ptr<MaterialParams>> params;
	params["style"] = std::make_shared<PointStyleParams>();
	params["layers"] = std::make_shared<LayersParams>();
	params["clipping"] = std::make_shared<ClippingParams>();
	params["light"] = std::make_shared<LightParams>();
	return ShaderBuffer(shader, vao, vbo, params);
};

void PointMaterial::update(ShaderBuffer &shaderBuffer, Geometry &geometry) {
	auto trianglesGeometry = dynamic_cast<TrianglesGeometry*>(&geometry);

	if (trianglesGeometry) {
		auto &ps = trianglesGeometry->_m.points;
		std::vector<Vertex> vertices(ps.size());
		for (int i = 0; i < ps.size(); ++i) {
			auto &v = ps[i];

			vertices[i] = { 
				.vertexIndex = i,
				.position = glm::vec3(v.x, v.y, v.z),
				.size = 1.f
			};
		}

		// shaderBuffer.rawData.clear();
		// shaderBuffer.rawData.resize(vertices.size() * sizeof(Vertex));
		// std::memcpy(shaderBuffer.rawData.data(), vertices.data(), shaderBuffer.rawData.size());

		shaderBuffer.nelements = vertices.size();
		glBindVertexArray(shaderBuffer.vao());
		glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer.vbo());
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		// glBufferData(GL_ARRAY_BUFFER, shaderBuffer.rawData.size(), shaderBuffer.rawData.data(), GL_STATIC_DRAW);
	}

}

void PointMaterial::init() {

	_params["style"] = std::make_shared<PointStyleParams>();
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


	// VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOVec3(shader.id, "aPos", sizeof(Vertex), (void*)offsetof(Vertex, position));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(Vertex), (void*)offsetof(Vertex, size));

}

void PointMaterial::push() {	

	std::vector<Vertex> vertices(ps.size());
	for (int i = 0; i < ps.size(); ++i) {
		auto &v = ps[i];

		vertices[i] = { 
			.vertexIndex = i,
			.position = glm::vec3(v.x, v.y, v.z),
			.size = 1.f
		};
	}

	writeVBOBuffer(vertices);
}



void PointMaterial::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	setPosition(position);

	for (auto &[paramsName, params] : _params)
		params->apply(shader);


	glDrawArrays(GL_POINTS, 0, ps.size());
}

void PointMaterial::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	clearPoints();
}

void PointMaterial::clean() {
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