#include "point_set_renderer.h"

void PointSetRenderer::setAttribute(std::vector<float> attributeData) {
	_attributeData = attributeData;

	// Get bounds (min-max)
	float min = std::numeric_limits<float>::max(); 
	float max = std::numeric_limits<float>::min();
	for (auto x : attributeData) {
		if (x < min)
			min = x;
		if (x > max)
			max = x;
	}

	// Update min/max
	shader.use();
	shader.setFloat2("attributeDataMinMax", glm::vec2(min, max));

	// Update sample
	glBindBuffer(GL_TEXTURE_BUFFER, pointAttributeBuffer);
	glBufferData(GL_TEXTURE_BUFFER, _attributeData.size() * sizeof(float), _attributeData.data(), GL_STATIC_DRAW);
}

void PointSetRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Init buffers
	glGenBuffers(1, &cellBaryBuffer);
	glGenBuffers(1, &pointAttributeBuffer);


	// Init textures
	glGenTextures(1, &cellBaryTexture);
	glGenTextures(1, &pointAttributeTexture);

	// Set up texture units
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_BUFFER, cellBaryTexture);
	glUniform1i(glGetUniformLocation(shader.id, "bary"), 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, pointAttributeTexture);
	glUniform1i(glGetUniformLocation(shader.id, "attributeData"), 2);


	// Vertex attributes
	GLuint vertexIndexLocation = glGetAttribLocation(shader.id, "vertexIndex");
	GLuint positionLocation = glGetAttribLocation(shader.id, "aPos");
	GLuint sizeLocation = glGetAttribLocation(shader.id, "sizeScale");

	glEnableVertexAttribArray(vertexIndexLocation);
	glVertexAttribIPointer(vertexIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, index));

	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

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

	glBindBuffer(GL_TEXTURE_BUFFER, cellBaryBuffer);
	glBufferData(GL_TEXTURE_BUFFER, _barys.size() * sizeof(float), _barys.data(), GL_STATIC_DRAW);
	glActiveTexture(GL_TEXTURE0 + 1); 
	glBindTexture(GL_TEXTURE_BUFFER, cellBaryTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, cellBaryBuffer);

	glBindBuffer(GL_TEXTURE_BUFFER, pointAttributeBuffer);
	glBufferData(GL_TEXTURE_BUFFER, _attributeData.size() * sizeof(float), _attributeData.data(), GL_STATIC_DRAW);
	glActiveTexture(GL_TEXTURE0 + 2); 
	glBindTexture(GL_TEXTURE_BUFFER, pointAttributeTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, pointAttributeBuffer);

	// Refresh attribute data if needed
	_attributeData.resize(ps.size(), 0.0f);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "push end in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
	std::cout << "point set has: " << ps.size() << " vertices." << std::endl;
}

void PointSetRenderer::render(glm::vec3 &position) {

	glBindVertexArray(VAO);

	// TODO is really necessary ????!!! see!!
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_BUFFER, cellBaryTexture);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, pointAttributeTexture);

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