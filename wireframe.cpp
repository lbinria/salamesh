#include "wireframe.h"



void Wireframe::setAttribute(std::vector<float> attributeData) {
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
	glBindBuffer(GL_TEXTURE_BUFFER, cellAttributeBuffer);
	glBufferData(GL_TEXTURE_BUFFER, _attributeData.size() * sizeof(float), _attributeData.data(), GL_STATIC_DRAW);
}

void Wireframe::changeAttribute(std::string name, int element) {
	// Change attribute
	shader.use();
	shader.setInt("attr_element", element);

	// Load attribute from mesh
	CellAttribute<double> a(name, attributes, m, -1);
	
	std::vector<float> converted_attribute_data(a.ptr->data.size());
	std::transform(a.ptr->data.begin(), a.ptr->data.end(), converted_attribute_data.begin(), [](double x) { return static_cast<float>(x);});

	setAttribute(converted_attribute_data);

	// for (int i = 0; i < 1000; i++) {
	// 	int idx = rand() % _highlights.size();
	// 	_highlights[idx] = rand() % 100 /100.f;
	// }
	// std::memcpy(ptr, _highlights.data(), _highlights.size() * sizeof(float));
}

void Wireframe::init() {

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::cout << "gen VAO." << std::endl;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

    std::cout << "gen VBO." << std::endl;


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

    std::cout << "Push data to buffer." << std::endl;

	glBufferData(GL_ARRAY_BUFFER, v_vertices.size() * sizeof(Vertex), v_vertices.data(), GL_STATIC_DRAW);

	std::cout << "gen cell buffer." << std::endl;

	// 
	glGenBuffers(1, &cellBaryBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, cellBaryBuffer);
	glBufferData(GL_TEXTURE_BUFFER, _barys.size() * sizeof(float), _barys.data(), GL_STATIC_DRAW);

	glGenTextures(1, &cellBaryTexture);
	glActiveTexture(GL_TEXTURE0 + 1); 
	glBindTexture(GL_TEXTURE_BUFFER, cellBaryTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, cellBaryBuffer);

	// 
	glGenBuffers(1, &cellAttributeBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, cellAttributeBuffer);
	_attributeData.resize(m.ncells(), 0.0f);
	glBufferData(GL_TEXTURE_BUFFER, _attributeData.size() * sizeof(float), _attributeData.data(), GL_STATIC_DRAW);

	glGenTextures(1, &cellAttributeTexture);
	glActiveTexture(GL_TEXTURE0 + 2); 
	glBindTexture(GL_TEXTURE_BUFFER, cellAttributeTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, cellAttributeBuffer);

	// --------
	// Highlight
	glGenBuffers(1, &cellHighlightBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, cellHighlightBuffer);
	_highlights.resize(m.ncells(), 0.0f);
	glBufferData(GL_TEXTURE_BUFFER, _highlights.size() * sizeof(float), _highlights.data(), GL_DYNAMIC_DRAW);

	// Allocate persistent storage
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_TEXTURE_BUFFER, _highlights.size() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	highlightsPtr = glMapBufferRange(GL_TEXTURE_BUFFER, 0, _highlights.size() * sizeof(float), flags);

	// void *ptr = glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
	// if (ptr) {
	// 	std::memcpy(ptr, _highlights.data(), _highlights.size() * sizeof(float));
	// 	glUnmapBuffer(GL_TEXTURE_BUFFER);
	// }

	glGenTextures(1, &cellHighlightTexture);
	glActiveTexture(GL_TEXTURE0 + 3); 
	glBindTexture(GL_TEXTURE_BUFFER, cellHighlightTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, cellHighlightBuffer);

	// --------
	// Filter
	glGenBuffers(1, &cellFilterBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, cellFilterBuffer);
	_filters.resize(m.ncells(), 0.0f);
	glBufferData(GL_TEXTURE_BUFFER, _filters.size() * sizeof(float), _filters.data(), GL_DYNAMIC_DRAW);

	// Allocate persistent storage
	glBufferStorage(GL_TEXTURE_BUFFER, _filters.size() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	filtersPtr = glMapBufferRange(GL_TEXTURE_BUFFER, 0, _filters.size() * sizeof(float), flags);

	glGenTextures(1, &cellFilterTexture);
	glActiveTexture(GL_TEXTURE0 + 4); 
	glBindTexture(GL_TEXTURE_BUFFER, cellFilterTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, cellFilterBuffer);

	// Set up texture units
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_BUFFER, cellBaryTexture);
	glUniform1i(glGetUniformLocation(shader.id, "bary"), 1);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, cellAttributeTexture);
	glUniform1i(glGetUniformLocation(shader.id, "attributeData"), 2);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, cellHighlightTexture);
	glUniform1i(glGetUniformLocation(shader.id, "highlight"), 3);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, cellFilterTexture);
	glUniform1i(glGetUniformLocation(shader.id, "_filter"), 4);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);


    std::cout << "mesh setup..." << std::endl;


	GLuint positionLocation = glGetAttribLocation(shader.id, "aPos");
	GLuint sizeLocation = glGetAttribLocation(shader.id, "size");
	GLuint normalLocation = glGetAttribLocation(shader.id, "normal");
	GLuint heightsLocation = glGetAttribLocation(shader.id, "aHeights");
	GLuint facetIndexLocation = glGetAttribLocation(shader.id, "facetIndex");
	GLuint cellIndexLocation = glGetAttribLocation(shader.id, "cellIndex");

	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(sizeLocation);
	glVertexAttribPointer(sizeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, size));

	glEnableVertexAttribArray(normalLocation);
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(heightsLocation);
	glVertexAttribPointer(heightsLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, heights));

	glEnableVertexAttribArray(facetIndexLocation);
	glVertexAttribIPointer(facetIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));

	glEnableVertexAttribArray(cellIndexLocation);
	glVertexAttribIPointer(cellIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, cellIndex));


	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "mesh setup in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;

}

void Wireframe::push() {
    std::cout << "push start." << std::endl;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	ncells = m.ncells();

	v_vertices.resize(m.nfacets() * 2 /* 2 tri per facet */ * 3 /* 3 points per tri */);

	// Cell properties
	std::chrono::steady_clock::time_point begin_barys = std::chrono::steady_clock::now();

	_attributeData.resize(m.ncells());
	for (int i = 0; i < _attributeData.size(); i++)
		_attributeData[i] = 0.f;


	// (8ms -> 3ms)
	_barys.resize(m.ncells() * 3);
	const int size = m.cells.size() / 8;
	for (int ci = 0; ci < size; ++ci) {
		// Compute bary
		const int off = ci * 8;
		const vec3 &v0 = m.points[m.cells[off]];	
		const vec3 &v1 = m.points[m.cells[off + 1]];
		const vec3 &v2 = m.points[m.cells[off + 2]];
		const vec3 &v3 = m.points[m.cells[off + 3]];
		const vec3 &v4 = m.points[m.cells[off + 4]];
		const vec3 &v5 = m.points[m.cells[off + 5]];
		const vec3 &v6 = m.points[m.cells[off + 6]];
		const vec3 &v7 = m.points[m.cells[off + 7]];

		_barys[ci * 3] = (v0.x + v1.x + v2.x + v3.x + v4.x + v5.x + v6.x + v7.x) / 8;
		_barys[ci * 3 + 1] = (v0.y + v1.y + v2.y + v3.y + v4.y + v5.y + v6.y + v7.y) / 8;
		_barys[ci * 3 + 2] = (v0.z + v1.z + v2.z + v3.z + v4.z + v5.z + v6.z + v7.z) / 8;
	}

	std::chrono::steady_clock::time_point end_barys = std::chrono::steady_clock::now();


	std::chrono::steady_clock::time_point begin_facets = std::chrono::steady_clock::now();


	int i = 0;
	const auto ref = reference_cells[m.cell_type];
	constexpr int verts[2][3] = {{0, 1, 3}, {2, 3, 1}};
	
	for (int ci = 0; ci < m.ncells(); ++ci) {
		for (int lfi = 0; lfi < 6; ++lfi) {
			// Get 4 points of facet
			const vec3 points[4] = {
				m.points[m.cells[ci * 8 + ref.facets[lfi * 4]]],
				m.points[m.cells[ci * 8 + ref.facets[lfi * 4 + 1]]],
				m.points[m.cells[ci * 8 + ref.facets[lfi * 4 + 2]]],
				m.points[m.cells[ci * 8 + ref.facets[lfi * 4 + 3]]]
			};

			// vec3 n = UM::normal(points, 4);
			
			vec3 res{0, 0, 0};
			const vec3 bary = (points[0] + points[1] + points[2] + points[3]);
			res += cross(points[0]-bary, points[1]-bary);
			res += cross(points[1]-bary, points[2]-bary);
			res += cross(points[2]-bary, points[3]-bary);
			res += cross(points[3]-bary, points[0]-bary);
			const vec3 n = res.normalized();

			for (int t = 0; t < 2; ++t) {

				const double a = (points[verts[t][1]] - points[verts[t][0]]).norm();
				const double b = (points[verts[t][2]] - points[verts[t][1]]).norm();
				const double c = (points[verts[t][2]] - points[verts[t][0]]).norm();

				const double side_lengths[] = {a, b, c};
				const double s = a + b + c;
				const double area = sqrt(s * (s - a) * (s - b) * (s - c));

				for (int j = 0; j < 3; ++j) {
					const auto v = points[verts[t][j]];

					// Compute height
					const double h = area / (.5 * side_lengths[(j + 1) % 3]);

					glm::vec3 edge(0.f, 0.f, 0.f);
					edge[j] = h;

					v_vertices[i] = { 
						glm::vec3(v.x, v.y, v.z),
						0.002f,
						glm::vec3(n.x, n.y, n.z),
						edge,
						ci * 6 + lfi,
						ci
					};


					++i;
				}
			}

		}
	}

	std::chrono::steady_clock::time_point end_facets = std::chrono::steady_clock::now();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "push end in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
    std::cout << "compute bary in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_barys - begin_barys).count() << "ms" << std::endl;
    std::cout << "compute facets in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_facets - begin_facets).count() << "ms" << std::endl;

	std::cout << "mesh has: " << m.nverts() << " vertices." << std::endl;
	std::cout << "mesh has: " << m.nfacets() << " facets." << std::endl;
	std::cout << "mesh has: " << m.ncells() << " cells." << std::endl;
	std::cout << "should draw: " << v_vertices.size() << " vertices." << std::endl;
}

void Wireframe::bind() {
    glBindVertexArray(VAO);
}

void Wireframe::render() {


    shader.use();

    // Draw	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	shader.setMat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, v_vertices.size());
	// glDrawArrays(GL_POINTS, 0, v_vertices.size());
}

void Wireframe::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	shader.clean();
}