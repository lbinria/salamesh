#include "point_set_renderer.h"

void PointSetRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLuint positionLocation = glGetAttribLocation(shader.id, "aPos");
	GLuint sizeLocation = glGetAttribLocation(shader.id, "sizeScale");

	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(sizeLocation);
	glVertexAttribPointer(sizeLocation, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, size));

	setPointSize(4.0f); // Default point size
}

void PointSetRenderer::push() {
    std::cout << "push start." << std::endl;



	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	vertices.resize(ps.size());

	for (int i = 0; i < ps.size(); ++i) {
		auto p = ps[i];
		vertices[i] = { 
			glm::vec3(p.x, p.y, p.z),
			1.f
		};
	}

	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);


	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "push end in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
	std::cout << "point set has: " << ps.size() << " vertices." << std::endl;
}

void PointSetRenderer::render(glm::vec3 &position) {

	glBindVertexArray(VAO);

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