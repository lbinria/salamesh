#include "point_set_renderer.h"
#include "../../helpers/graphic_api.h"

void PointSetRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	
	// For the moment don't use persistent mapped memory
	sl::createTBO(bufAttr, texAttr, 2);
	sl::createTBO(bufHighlight, tboHighlight, 3);
	sl::createTBO(bufFilter, tboFilter, 4);

	shader.use();
	shader.setInt("attrBuf", 2);
	shader.setInt("highlightBuf", 3);
	shader.setInt("filterBuf", 4);

	// VBO
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOVec3(shader.id, "aPos", sizeof(Vertex), (void*)offsetof(Vertex, position));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(Vertex), (void*)offsetof(Vertex, size));
	// sl::createVBOVec3(shader.id, "normal", sizeof(Vertex), (void*)offsetof(Vertex, normal));

}

void PointSetRenderer::push() {	

	// // TODO clean remove that, not used anymore !
	// std::vector<glm::vec3> normals(ps.size(), glm::vec3(0.));

	// if (_m) {

	// 	auto &m = *_m;

	// 	for (auto &f : m.iter_facets()) {

	// 		// Compute facet normal
	// 		int nbv = f.size();

	// 		std::vector<vec3> pos(f.size());
	// 		for (int lv = 0; lv < f.size(); ++lv) {
	// 			pos[lv] = f.vertex(lv).pos();
	// 		}
	// 		auto n = UM::geo::normal(pos.data(), nbv);

	// 		// Set facet normal to all vertices of facet
	// 		for (int lv = 0; lv < f.size(); ++lv) {
	// 			auto v = f.vertex(lv);
	// 			normals[v] += glm::vec3(n.x, n.y, n.z);
	// 			normals[v] = glm::normalize(normals[v]);
	// 		}
	// 	}

	// 	for (int i = 0; i < normals.size(); ++i) {
	// 		normals[i] = glm::normalize(normals[i]);
	// 	}
	// }

	std::vector<Vertex> vertices(ps.size());
	for (int i = 0; i < ps.size(); ++i) {
		auto &v = ps[i];

		vertices[i] = { 
			.vertexIndex = i,
			.position = glm::vec3(v.x, v.y, v.z),
			.size = 1.f
			// .normal = normals[i]
		};
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void PointSetRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texColorMap);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, tboHighlight);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, tboFilter);


	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	
	// Draw	
	shader.use();
	shader.setMat4("model", model);

	glDrawArrays(GL_POINTS, 0, ps.size());
}

void PointSetRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteBuffers(1, &bufAttr);
	glDeleteTextures(1, &texAttr);
	glDeleteBuffers(1, &bufHighlight);
	glDeleteTextures(1, &tboHighlight);
	glDeleteBuffers(1, &bufFilter);
	glDeleteTextures(1, &tboFilter);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}