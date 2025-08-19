#include "tet_renderer.h"


void TetRenderer::setAttribute(ContainerBase *ga, int element) {
	// Set attribute element to shader
	shader.use();
	shader.setInt("attrElement", element);

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


void TetRenderer::setAttribute(std::vector<float> attributeData) {

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

void TetRenderer::init() {

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

	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	
	glGenBuffers(1, &bufHighlight);
	glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	glBufferStorage(GL_TEXTURE_BUFFER, _m.ncells() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	ptrHighlight = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, _m.ncells() * sizeof(float), flags);

	glGenTextures(1, &texHighlight);
	glActiveTexture(GL_TEXTURE0 + 3); 
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);


	glGenBuffers(1, &bufFacetHighlight);
	glBindBuffer(GL_TEXTURE_BUFFER, bufFacetHighlight);
	glBufferStorage(GL_TEXTURE_BUFFER, _m.nfacets() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	ptrFacetHighlight = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, _m.nfacets() * sizeof(float), flags);

	glGenTextures(1, &texFacetHighlight);
	glActiveTexture(GL_TEXTURE0 + 4); 
	glBindTexture(GL_TEXTURE_BUFFER, texFacetHighlight);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufFacetHighlight);

	glGenBuffers(1, &bufFilter);
	glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
	glBufferStorage(GL_TEXTURE_BUFFER, _m.ncells() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	ptrFilter = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, _m.ncells() * sizeof(float), flags);

	glGenTextures(1, &texFilter);
	glActiveTexture(GL_TEXTURE0 + 5); 
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
	glBindTexture(GL_TEXTURE_BUFFER, texFacetHighlight);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	shader.use();
	shader.setInt("bary", 1);
	shader.setInt("attributeData", 2);
	shader.setInt("highlightBuf", 3);
	shader.setInt("facetHighlightBuf", 4);
	shader.setInt("filterBuf", 5);

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

void TetRenderer::push() {

	#ifdef _DEBUG
    std::cout << "push start." << std::endl;
	#endif

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	nverts = _m.nfacets() * 3 /* 3 points per tri */;
	std::vector<Vertex> vertices(nverts);

	// Cell properties

	// (8ms -> 3ms)
	std::vector<float> barys(_m.ncells() * 3);

	const int size = _m.cells.size() / 4;
	for (int ci = 0; ci < size; ++ci) {
		// Compute bary
		const int off = ci * 4;
		const vec3 &v0 = _m.points[_m.cells[off]];	
		const vec3 &v1 = _m.points[_m.cells[off + 1]];
		const vec3 &v2 = _m.points[_m.cells[off + 2]];
		const vec3 &v3 = _m.points[_m.cells[off + 3]];

		barys[ci * 3] = (v0.x + v1.x + v2.x + v3.x) / 4;
		barys[ci * 3 + 1] = (v0.y + v1.y + v2.y + v3.y) / 4;
		barys[ci * 3 + 2] = (v0.z + v1.z + v2.z + v3.z) / 4;
	}

	std::chrono::steady_clock::time_point end_barys = std::chrono::steady_clock::now();


	std::chrono::steady_clock::time_point begin_facets = std::chrono::steady_clock::now();


	int i = 0;
	
	for (int ci = 0; ci < _m.ncells(); ++ci) {
		for (int lfi = 0; lfi < 4; ++lfi) {


			// Compute normal
			Volume::Facet f(_m, _m.facet(ci, lfi));
			Triangle3 t = f;
			auto n = t.normal();

			const double a = (f.vertex(1).pos() - f.vertex(0).pos()).norm();
			const double b = (f.vertex(2).pos() - f.vertex(1).pos()).norm();
			const double c = (f.vertex(2).pos() - f.vertex(0).pos()).norm();

			const double side_lengths[] = {a, b, c};
			const double s = a + b + c;
			const double area = sqrt(s * (s - a) * (s - b) * (s - c));

			for (int lv = 0; lv < 3; ++lv) {
				auto v = f.vertex(lv);
				auto &p = v.pos();

				// Compute height
				const double h = area / (.5 * side_lengths[(lv + 1) % 3]);
				glm::vec3 edge(0.f, 0.f, 0.f);
				edge[lv] = h;

				vertices[i] = { 
					vertexIndex: v,
					position: glm::vec3(p.x, p.y, p.z),
					size: 1.f,
					normal: glm::vec3(n.x, n.y, n.z),
					heights: edge,
					facetIndex: f,
					cellIndex: ci
				};

				++i;
			}
		}
	}

	std::chrono::steady_clock::time_point end_facets = std::chrono::steady_clock::now();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	#ifdef _DEBUG
    std::cout << "push end in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
    std::cout << "compute bary in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_barys - begin_barys).count() << "ms" << std::endl;
    std::cout << "compute facets in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_facets - begin_facets).count() << "ms" << std::endl;

	
	std::cout << "mesh has: " << _m.nverts() << " vertices." << std::endl;
	std::cout << "mesh has: " << _m.nfacets() << " facets." << std::endl;
	std::cout << "mesh has: " << _m.ncells() << " cells." << std::endl;
	std::cout << "should draw: " << vertices.size() << " vertices." << std::endl;
	#endif

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_TEXTURE_BUFFER, bufBary);
	glBufferData(GL_TEXTURE_BUFFER, barys.size() * sizeof(float), barys.data(), GL_STATIC_DRAW);

	// // Update pointers
	// GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	// // Remap highlight
	// if(bufHighlight != 0) {
	// 	glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	// 	ptrHighlight = nullptr;            // forget old pointer
	// 	glDeleteBuffers(1, &bufHighlight);
	// }

	// glGenBuffers(1, &bufHighlight);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	// glBufferStorage(GL_TEXTURE_BUFFER, _m.ncells() * sizeof(float), nullptr, flags);
	// ptrHighlight = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, _m.ncells() * sizeof(float), flags);

	// glActiveTexture(GL_TEXTURE0 + 3); 
	// glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);
	// TODO remap facet highlight, filter...

}

void TetRenderer::render(glm::vec3 &position) {

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
	glBindTexture(GL_TEXTURE_BUFFER, texFacetHighlight);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	
    // Draw	
    shader.use();
	shader.setMat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, nverts);
}

void TetRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// Unmap highlight
	if (ptrHighlight) {
		glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
		glUnmapBuffer(GL_TEXTURE_BUFFER);
		ptrHighlight = nullptr;
	}
	// Unmap facet highlight
	if (ptrFacetHighlight) {
		glBindBuffer(GL_TEXTURE_BUFFER, bufFacetHighlight);
		glUnmapBuffer(GL_TEXTURE_BUFFER);
		ptrFacetHighlight = nullptr;
	}
	// Unmap filter
	if (ptrFilter) {
		glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
		glUnmapBuffer(GL_TEXTURE_BUFFER);
		ptrFilter = nullptr;
	}

	glDeleteBuffers(1, &bufBary);
	glDeleteTextures(1, &texBary);
	glDeleteBuffers(1, &bufAttr);
	glDeleteTextures(1, &texAttr);
	glDeleteBuffers(1, &bufHighlight);
	glDeleteTextures(1, &texHighlight);
	glDeleteBuffers(1, &bufFacetHighlight);
	glDeleteTextures(1, &texFacetHighlight);
	glDeleteBuffers(1, &bufFilter);
	glDeleteTextures(1, &texFilter);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}