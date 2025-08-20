#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "core/renderer.h"
#include "core/model.h"
#include "core/element.h"
#include "mesh_shader.h"
#include "vertex.h"

using namespace UM;

struct VolumeRenderer : public IRenderer {

	VolumeRenderer(Volume &m) : 
		_m(m),
		shader("shaders/volume.vert", "shaders/volume.frag")
		{
			shader.setColor({0.8f, 0.f, 0.2f}); // TODO here use a setting default point color
		}


	// TODO eventually merge init / update
	void init();
	void render(glm::vec3 &position);
	
	void push(std::vector<Vertex> &vertices) {

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
		// pushHighlights();
		// Push filters if there is any
		// pushFilters();
	}


	void clean();

	void setTexture(unsigned int tex) { texColorMap = tex; }

	void setVisible(bool v) {
		visible = v;
	}

	bool getVisible() const {
		return visible;
	}

	void setHighlight(int idx, float highlight) {
		ptrHighlight[idx] = highlight;
	}

	void setHighlight(std::vector<float> highlights) {
		std::memcpy(ptrHighlight, highlights.data(), highlights.size() * sizeof(float));
	}

	void setFilter(int idx, bool filter) {
		ptrFilter[idx] = filter ? 1.f : 0.f;
	}

	float* &getFilterPtr() {
		return ptrFilter;
	}

	void setFacetHighlight(int idx, float highlight) {
		ptrFacetHighlight[idx] = highlight;
	}

	void setFacetHighlight(std::vector<float> highlights) {
		std::memcpy(ptrFacetHighlight, highlights.data(), highlights.size() * sizeof(float));
	}

	void setAttribute(std::vector<float> attributeData);
	void setAttribute(ContainerBase *ga, int element);

	MeshShader shader;

	protected:

	Volume &_m;

	bool visible = true;

	unsigned int VAO, VBO; // Buffers
	unsigned int bufBary, bufHighlight, bufFacetHighlight, bufAttr, bufFilter; // Sample buffers
	unsigned int texColorMap, texBary, texHighlight, texFacetHighlight, texAttr, texFilter; // Textures

	float *ptrAttr;
	float *ptrHighlight;
	float *ptrFacetHighlight;
	float *ptrFilter;

	int nverts = 0;
};