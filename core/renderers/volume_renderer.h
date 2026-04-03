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

#include "mesh_renderer.h"
#include "../element.h"

using namespace UM;

struct VolumeRenderer : public MeshRenderer {

	struct Vertex {
		int vertexIndex;
		glm::vec3 position;
		float size;
		glm::vec3 normal; 
		glm::vec3 heights;
		int facetIndex;
		int cellIndex;
	};

	VolumeRenderer(std::string name, Volume &m) : 
		MeshRenderer(name, Shader(sl::shadersPath("volume.vert"), sl::shadersPath("volume.frag"))),
		_m(m)
		{}


	void init() override;
	void render(RendererView &rv, glm::vec3 &position) override;
	void clear() override;
	void clean() override;

	std::unique_ptr<RendererView> getDefaultView() override { return std::make_unique<VolumeRendererView>(); }

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
	// TODO remove after refacto of volumes 
	unsigned int bufAttr;
	unsigned int texAttr;
	unsigned int texColorMap;

	void doLoadState(json &j) override {
		MeshRenderer::doLoadState(j);
	}
	void doSaveState(json &j) const override {
		MeshRenderer::doSaveState(j);
	}

};