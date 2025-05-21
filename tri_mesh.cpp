#include "tri_mesh.h"

// Model init
void TriMesh::init() {

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);  
	glBufferData(GL_ARRAY_BUFFER, sizeof(v_vertices.data()) * v_vertices.size(), v_vertices.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(v_indices.data()) * v_indices.size(), v_indices.data(), GL_DYNAMIC_DRAW);

	// Bind vertex attribute to float vec3 at location 0, no space between data (stride=0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Bind color vertex attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Bind tex coords vertex attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Bind edge distance vertex attribute
	glVertexAttribPointer(3, 6, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
}

void TriMesh::bind() {
    glBindVertexArray(VAO);
}

void TriMesh::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	shader.clean();
}

void TriMesh::to_gl(Triangles &m) {

	v_vertices.resize(14 * m.nverts());
	v_indices.resize(3 * m.nfacets());

	for (auto v : m.iter_vertices()) {
		// pos
		v_vertices[v * 14 + 0] = v.pos().x;
		v_vertices[v * 14 + 1] = v.pos().y;
		v_vertices[v * 14 + 2] = v.pos().z;
		// color
		v_vertices[v * 14 + 3] = rand() % 100 / 100.;
		v_vertices[v * 14 + 4] = rand() % 100 / 100.;
		v_vertices[v * 14 + 5] = rand() % 100 / 100.;
		// tex coords
		v_vertices[v * 14 + 6] = 0.f;
		v_vertices[v * 14 + 7] = 0.f;
		// edge dist
		v_vertices[v * 14 + 8] = 0.f;
		v_vertices[v * 14 + 9] = 0.f;
		v_vertices[v * 14 + 10] = 0.f;
		v_vertices[v * 14 + 11] = 0.f;
		v_vertices[v * 14 + 12] = 0.f;
		v_vertices[v * 14 + 13] = 0.f;
	}

	// Direct copy
	v_indices = m.facets;

}

void TriMesh::render() {
    shader.use();

    // Draw	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	shader.setMat4("model", model);

	glDrawElements(GL_TRIANGLES, v_indices.size(), GL_UNSIGNED_INT, 0);
	// glDrawArrays(GL_POINTS, 0, m.nverts());
	// glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	// glDrawArrays(GL_TRIANGLES, 0, 36);
	// glDrawArrays(GL_POINTS, 0, 36);
	// glDrawArrays(GL_LINES, 0, 36);
}