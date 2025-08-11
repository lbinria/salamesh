#include "halfedge_renderer.h"

void HalfedgeRenderer::setAttribute(ContainerBase *ga) {

	// Prepare data
	std::vector<float> converted_attribute_data;

	// Transform data
	if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {

		converted_attribute_data.resize(a->data.size());
		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](double x) { return static_cast<float>(x);});

	} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
		
		converted_attribute_data.resize(a->data.size());
		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

	} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
		
		converted_attribute_data.resize(a->data.size());
		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

	} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(ga)) {

		converted_attribute_data.resize(a->data.size());
		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

	}

	// Set attribute data to shader
	setAttribute(converted_attribute_data);
}

void HalfedgeRenderer::setAttribute(std::vector<float> attributeData) {
	// Get bounds (min-max)
	float min = std::numeric_limits<float>::max(); 
	float max = std::numeric_limits<float>::min();
	for (auto x : attributeData) {
		min = std::min(min, x);
		max = std::max(max, x);
	}

	// Update min/max
	shader.use();
	shader.setFloat2("attrRange", glm::vec2(min, max));

	// Update sample
	glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	glBufferData(GL_TEXTURE_BUFFER, attributeData.size() * sizeof(float), attributeData.data(), GL_STATIC_DRAW);
}

void HalfedgeRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	
	// glGenBuffers(1, &bufAttr);
	// glGenTextures(1, &texAttr);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	// glActiveTexture(GL_TEXTURE0 + 2);
	// glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufAttr);

	// GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	// // Filter
	// glGenBuffers(1, &bufFilter);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);

	// glBufferStorage(GL_TEXTURE_BUFFER, ps.size() * sizeof(float), nullptr, flags);
	// // Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	// ptrFilter = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, ps.size() * sizeof(float), flags);

	// glGenTextures(1, &texFilter);
	// glActiveTexture(GL_TEXTURE0 + 4); 
	// glBindTexture(GL_TEXTURE_BUFFER, texFilter);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufFilter);

	// // Highlight
	// glGenBuffers(1, &bufHighlight);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);

	// glBufferStorage(GL_TEXTURE_BUFFER, ps.size() * sizeof(float), nullptr, flags);
	// // Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	// ptrHighlight = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, ps.size() * sizeof(float), flags);

	// glGenTextures(1, &texHighlight);
	// glActiveTexture(GL_TEXTURE0 + 3); 
	// glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);


	shader.use();
	// shader.setInt("attrBuf", 2);
	// shader.setInt("highlightBuf", 3);
	// shader.setInt("filterBuf", 4);

	// VBO
	GLuint p0Loc = glGetAttribLocation(shader.id, "aP0");
	glEnableVertexAttribArray(p0Loc);
	glVertexAttribPointer(p0Loc, 3, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, P0));

	GLuint p1Loc = glGetAttribLocation(shader.id, "aP1");
	glEnableVertexAttribArray(p1Loc);
	glVertexAttribPointer(p1Loc, 3, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, P1));

	GLuint sideLoc = glGetAttribLocation(shader.id, "aSide");
	glEnableVertexAttribArray(sideLoc);
	glVertexAttribPointer(sideLoc, 1, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, side));

	GLuint endLoc = glGetAttribLocation(shader.id, "aEnd");
	glEnableVertexAttribArray(endLoc);
	glVertexAttribPointer(endLoc, 1, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, end));

	// GLuint vertexIndexLocation = glGetAttribLocation(shader.id, "vertexIndex");
	// glEnableVertexAttribArray(vertexIndexLocation);
	// glVertexAttribIPointer(vertexIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));

	// GLuint sizeLocation = glGetAttribLocation(shader.id, "sizeScale");
	// glEnableVertexAttribArray(sizeLocation);
	// glVertexAttribPointer(sizeLocation, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, size));

}

void HalfedgeRenderer::push() {	

	
	npoints = 24 * v.ncells() * 6;
	// std::vector<LineVert> vertices(npoints);
	std::vector<LineVert> vertices;

	for (auto &c : v.iter_cells()) {
		for (int f = 0; f < 6; ++f) {
			for (int i = 0; i < 4; ++i) {
				auto lc0 = reference_cells[1].facets[i % 4 + 4 * f];
				auto lc1 = reference_cells[1].facets[(i + 1) % 4 + 4 * f];
				auto v0 = c.corner(lc0).vertex();
				auto v1 = c.corner(lc1).vertex();
				auto p0 = v0.pos();
				auto p1 = v1.pos();
				
				// build the 4 “corner” vertices
				LineVert lv0{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), -1.0f, 0.0f};  // corner: start, left side
				LineVert lv1{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), +1.0f, 0.0f};  // corner: start, right side
				LineVert lv2{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), -1.0f, 1.0f};  // corner: end,   left side
				LineVert lv3{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), +1.0f, 1.0f};  // corner: end,   right side

				vertices.push_back(lv0);
				vertices.push_back(lv1);
				vertices.push_back(lv2);
				
				vertices.push_back(lv2);
				vertices.push_back(lv3);
				vertices.push_back(lv1);

			}
		}
	}

	// npoints = 24 * v.ncells();
	// // std::vector<LineVert> vertices(npoints);
	// std::vector<LineVert> vertices;

	// for (auto &c : v.iter_cells()) {
	// 	for (int f = 0; f < 6; ++f) {
	// 		for (int i = 0; i < 4; ++i) {
	// 			auto lc0 = reference_cells[1].facets[i % 4 + 4 * f];
	// 			auto lc1 = reference_cells[1].facets[(i + 1) % 4 + 4 * f];
	// 			auto v0 = c.corner(lc0).vertex();
	// 			auto v1 = c.corner(lc1).vertex();
	// 			auto p0 = v0.pos();
	// 			auto p1 = v1.pos();
				
	// 			// build the 4 “corner” vertices
	// 			LineVert lv0{glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), -1.0f, 0.0f};  // corner: start, left side
	// 			vertices.push_back(lv0);
	// 		}
	// 	}
	// }

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, npoints * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
}

void HalfedgeRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_1D, texColorMap);
	// glActiveTexture(GL_TEXTURE0 + 2);
	// glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	// glActiveTexture(GL_TEXTURE0 + 3);
	// glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	// glActiveTexture(GL_TEXTURE0 + 4);
	// glBindTexture(GL_TEXTURE_BUFFER, texFilter);


	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	// Draw	
    shader.use();
	shader.setMat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, npoints);
	// glDrawArrays(GL_POINTS, 0, npoints);
}

void HalfedgeRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// TODO clean buffers, textures, unmap ptr...

	shader.clean();
}