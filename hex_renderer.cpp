#include "hex_renderer.h"

void HexRenderer::changeAttribute(GenericAttributeContainer *ga, int element) {
	// Set attribute element to shader
	shader.use();
	shader.setInt("attr_element", element);

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


void HexRenderer::setAttribute(std::vector<float> attributeData) {

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

void HexRenderer::init() {

	// TODO maybe update buffer size of ptr on push ? move ncells somewher eelse

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glGenBuffers(1, &bufBary);
	glGenTextures(1, &texBary);
	glBindBuffer(GL_TEXTURE_BUFFER, bufBary);
	glActiveTexture(GL_TEXTURE0 + 1); 
	glBindTexture(GL_TEXTURE_BUFFER, texBary);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, bufBary);




	glGenBuffers(1, &bufAttr);
	glGenTextures(1, &texAttr);
	glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	glActiveTexture(GL_TEXTURE0 + 2); 
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufAttr);

	
	glGenBuffers(1, &bufHighlight);
	glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);

	_highlights.resize(hex.ncells(), 0.0f);
	// Allocate persistent storage
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_TEXTURE_BUFFER, _highlights.size() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	highlightsPtr = glMapBufferRange(GL_TEXTURE_BUFFER, 0, _highlights.size() * sizeof(float), flags);

	glGenTextures(1, &texHighlight);
	glActiveTexture(GL_TEXTURE0 + 3); 
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);


	glGenBuffers(1, &bufFilter);
	glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
	glBufferStorage(GL_TEXTURE_BUFFER, hex.ncells() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	ptrFilter = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, hex.ncells() * sizeof(float), flags);

	glGenTextures(1, &texFilter);
	glActiveTexture(GL_TEXTURE0 + 4); 
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufFilter);











	// Set up texture units
	
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_BUFFER, texBary);
	
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);


	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);


	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	shader.use();
	shader.setInt("bary", 1);
	shader.setInt("attributeData", 2);
	shader.setInt("highlight", 3);
	shader.setInt("_filter", 4);

	#ifdef _DEBUG
    std::cout << "vertex attrib setup..." << std::endl;
	#endif

	GLuint positionLocation = glGetAttribLocation(shader.id, "aPos");
	GLuint sizeLocation = glGetAttribLocation(shader.id, "size");
	GLuint normalLocation = glGetAttribLocation(shader.id, "normal");
	GLuint heightsLocation = glGetAttribLocation(shader.id, "aHeights");
	GLuint facetIndexLocation = glGetAttribLocation(shader.id, "facetIndex");
	GLuint cellIndexLocation = glGetAttribLocation(shader.id, "cellIndex");
	GLuint vertexIndexLocation = glGetAttribLocation(shader.id, "vertexIndex");

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

	glEnableVertexAttribArray(vertexIndexLocation);
	glVertexAttribIPointer(vertexIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));

	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	#ifdef _DEBUG
    std::cout << "mesh setup in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
	#endif



}

void HexRenderer::push() {

	#ifdef _DEBUG
    std::cout << "push start." << std::endl;
	#endif

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	nverts = hex.nfacets() * 2 /* 2 tri per facet */ * 3 /* 3 points per tri */;
	std::vector<Vertex> vertices(nverts);

	// Cell properties
	std::chrono::steady_clock::time_point begin_barys = std::chrono::steady_clock::now();

	// (8ms -> 3ms)
	std::vector<float> barys(hex.ncells() * 3);

	const int size = hex.cells.size() / 8;
	for (int ci = 0; ci < size; ++ci) {
		// Compute bary
		const int off = ci * 8;
		const vec3 &v0 = hex.points[hex.cells[off]];	
		const vec3 &v1 = hex.points[hex.cells[off + 1]];
		const vec3 &v2 = hex.points[hex.cells[off + 2]];
		const vec3 &v3 = hex.points[hex.cells[off + 3]];
		const vec3 &v4 = hex.points[hex.cells[off + 4]];
		const vec3 &v5 = hex.points[hex.cells[off + 5]];
		const vec3 &v6 = hex.points[hex.cells[off + 6]];
		const vec3 &v7 = hex.points[hex.cells[off + 7]];

		barys[ci * 3] = (v0.x + v1.x + v2.x + v3.x + v4.x + v5.x + v6.x + v7.x) / 8;
		barys[ci * 3 + 1] = (v0.y + v1.y + v2.y + v3.y + v4.y + v5.y + v6.y + v7.y) / 8;
		barys[ci * 3 + 2] = (v0.z + v1.z + v2.z + v3.z + v4.z + v5.z + v6.z + v7.z) / 8;
	}

	std::chrono::steady_clock::time_point end_barys = std::chrono::steady_clock::now();


	std::chrono::steady_clock::time_point begin_facets = std::chrono::steady_clock::now();


	int i = 0;
	const auto ref = reference_cells[hex.cell_type];
	constexpr int verts[2][3] = {{0, 1, 3}, {2, 3, 1}};
	
	for (int ci = 0; ci < hex.ncells(); ++ci) {
		for (int lfi = 0; lfi < 6; ++lfi) {
			// Get 4 points of facet
			const vec3 points[4] = {
				hex.points[hex.cells[ci * 8 + ref.facets[lfi * 4]]],
				hex.points[hex.cells[ci * 8 + ref.facets[lfi * 4 + 1]]],
				hex.points[hex.cells[ci * 8 + ref.facets[lfi * 4 + 2]]],
				hex.points[hex.cells[ci * 8 + ref.facets[lfi * 4 + 3]]]
			};

			const int vertices_ref[4] = {
				hex.cells[ci * 8 + ref.facets[lfi * 4]],
				hex.cells[ci * 8 + ref.facets[lfi * 4 + 1]],
				hex.cells[ci * 8 + ref.facets[lfi * 4 + 2]],
				hex.cells[ci * 8 + ref.facets[lfi * 4 + 3]]
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
					const auto vi = vertices_ref[verts[t][j]];

					// Compute height
					const double h = area / (.5 * side_lengths[(j + 1) % 3]);

					glm::vec3 edge(0.f, 0.f, 0.f);
					edge[j] = h;

					vertices[i] = { 
						vertexIndex: vi,
						position: glm::vec3(v.x, v.y, v.z),
						size: 1.f,
						normal: glm::vec3(n.x, n.y, n.z),
						heights: edge,
						facetIndex: ci * 6 + lfi,
						cellIndex: ci
					};

					++i;
				}
			}

		}
	}

	std::chrono::steady_clock::time_point end_facets = std::chrono::steady_clock::now();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	#ifdef _DEBUG
    std::cout << "push end in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
    std::cout << "compute bary in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_barys - begin_barys).count() << "ms" << std::endl;
    std::cout << "compute facets in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_facets - begin_facets).count() << "ms" << std::endl;

	
	std::cout << "mesh has: " << hex.nverts() << " vertices." << std::endl;
	std::cout << "mesh has: " << hex.nfacets() << " facets." << std::endl;
	std::cout << "mesh has: " << hex.ncells() << " cells." << std::endl;
	std::cout << "should draw: " << vertices.size() << " vertices." << std::endl;
	#endif

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_TEXTURE_BUFFER, bufBary);
	glBufferData(GL_TEXTURE_BUFFER, barys.size() * sizeof(float), barys.data(), GL_STATIC_DRAW);
}

void HexRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

    glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texColorMap);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_BUFFER, texBary);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	
    // Draw	
    shader.use();
	shader.setMat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, nverts);
}

void HexRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// TODO clean buffers, textures, unmap ptr...

	shader.clean();
}