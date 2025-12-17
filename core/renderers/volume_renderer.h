#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../../include/glm/glm.hpp"
#include "../../include/glm/gtc/matrix_transform.hpp"
#include "../../include/glm/gtc/type_ptr.hpp"

#include "../include/json.hpp"
using json = nlohmann::json;

#include "renderer.h"
#include "../element.h"

using namespace UM;

struct VolumeRenderer : public IRenderer {

	struct Vertex {
		int vertexIndex;
		glm::vec3 position;
		float size;
		glm::vec3 normal; 
		glm::vec3 heights;
		int facetIndex;
		int cellIndex;
	};

	VolumeRenderer(Volume &m) : 
		IRenderer(Shader("shaders/volume.vert", "shaders/volume.frag")),
		_m(m)
		{
			shader.use();
			shader.setFloat3("color", {0.71f, 0.71f, 0.71f});
		}


	void init();
	void render(glm::vec3 &position);
	void clean();

	int getRenderElementKind() override { return ElementKind::CELLS_ELT | ElementKind::CELL_FACETS_ELT; }

	protected:

	Volume &_m;

	void push_bary(std::vector<Vertex> &vertices) {

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Compute barys here
		int nvertsPerCell = _m.nverts_per_cell();
		std::vector<float> barys(_m.ncells() * 3);

		const int size = _m.ncells();
		for (int ci = 0; ci < size; ++ci) {
			// Compute bary
			const int off = ci * nvertsPerCell;
			vec3 b(0, 0, 0);

			for (int lv = 0; lv < nvertsPerCell; ++lv) {
				b += _m.points[_m.cells[off + lv]];
			}

			barys[ci * 3] = b.x / nvertsPerCell;
			barys[ci * 3 + 1] = b.y / nvertsPerCell;
			barys[ci * 3 + 2] = b.z / nvertsPerCell;
		}

		glBindBuffer(GL_TEXTURE_BUFFER, bufBary);
		glBufferData(GL_TEXTURE_BUFFER, barys.size() * sizeof(float), barys.data(), GL_STATIC_DRAW);

		// Push currently selected attribute => 
		// _m.attr_cells, _m.attr_corners...
		// pushAttrs();
		// Push highlights if there is any
		// updateHighlights();
		// Push filters if there is any
		// updateFilters();
	}

	private:

	unsigned int bufBary, texBary;

	void doLoadState(json &j) override {}
	void doSaveState(json &j) const override {}

};