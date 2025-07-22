#include "point_set_renderer.h"

void PointSetRenderer::changeAttribute(GenericAttributeContainer *ga) {
	// Set attribute element to shader
	shader.use();

	// Transform data
	if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {

		std::vector<float> converted_attribute_data(a->data.size());
		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](double x) { return static_cast<float>(x);});

		// Set attribute data to shader
		setAttribute(converted_attribute_data);
	} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
		
		setAttribute(a->data);

	} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
		
		std::vector<float> converted_attribute_data(a->data.size());
		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](int x) { return static_cast<float>(x);});
		setAttribute(converted_attribute_data);
	}
	// TODO vec2 / vec3
}

void PointSetRenderer::setAttribute(std::vector<float> attributeData) {
	// Get bounds (min-max)
	float min = std::numeric_limits<float>::max(); 
	float max = std::numeric_limits<float>::min();
	for (auto x : attributeData) {
		min = std::min(min, x);
		max = std::max(max, x);
	}

	// Update min/max
	shader.use();
	shader.setFloat2("attributeDataMinMax", glm::vec2(min, max));

	// Update sample
	glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	glBufferData(GL_TEXTURE_BUFFER, attributeData.size() * sizeof(float), attributeData.data(), GL_STATIC_DRAW);
}

void PointSetRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Init buffers / tex
	glGenBuffers(1, &bufBary);
	glGenTextures(1, &texBary);
	// Set up texture units
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_BUFFER, texBary);
	glUniform1i(glGetUniformLocation(shader.id, "bary"), 1);


	glGenBuffers(1, &bufAttr);
	glGenTextures(1, &texAttr);
	glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufAttr);
	glUniform1i(glGetUniformLocation(shader.id, "attributeData"), 2);


	// VBO
	GLuint vertexIndexLocation = glGetAttribLocation(shader.id, "vertexIndex");
	glEnableVertexAttribArray(vertexIndexLocation);
	glVertexAttribIPointer(vertexIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, index));

	GLuint positionLocation = glGetAttribLocation(shader.id, "aPos");
	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	GLuint sizeLocation = glGetAttribLocation(shader.id, "sizeScale");
	glEnableVertexAttribArray(sizeLocation);
	glVertexAttribPointer(sizeLocation, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, size));
}

void PointSetRenderer::push() {
    std::cout << "push start." << std::endl;



	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	vertices.resize(ps.size());

	for (int i = 0; i < ps.size(); ++i) {
		auto p = ps[i];
		vertices[i] = { 
			i,
			glm::vec3(p.x, p.y, p.z),
			1.f
		};
	}

	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// glBindBuffer(GL_TEXTURE_BUFFER, bufBary);
	// glBufferData(GL_TEXTURE_BUFFER, _barys.size() * sizeof(float), _barys.data(), GL_STATIC_DRAW);
	// glActiveTexture(GL_TEXTURE0 + 1); 
	// glBindTexture(GL_TEXTURE_BUFFER, texBary);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, bufBary);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "push end in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
	std::cout << "point set has: " << ps.size() << " vertices." << std::endl;
}

void PointSetRenderer::render(glm::vec3 &position) {

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texColorMap);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_BUFFER, texBary);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

    shader.use();

    // Draw	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	shader.setMat4("model", model);

	glDrawArrays(GL_POINTS, 0, vertices.size());
}

void PointSetRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shader.clean();
}