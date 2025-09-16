#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

#include "renderer.h"
#include "model.h"
#include "element.h"
#include "mesh_shader.h"
#include "vertex.h"
using namespace UM;

struct SurfaceRenderer : public IRenderer {
	
	SurfaceRenderer(Surface &m) : 
		_m(m),
		shader("shaders/surface.vert", "shaders/surface.frag")
		{
			shader.setColor({0.8f, 0.f, 0.2f}); // TODO here use a setting default mesh color
		}


	void init();
	void render(glm::vec3 &position);
	

	void clean();

	void push(std::vector<Vertex> &vertices) {

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Compute barys here
		int nvertsPerFacet = _m.facet_size(*_m.facets.begin());
		std::vector<float> barys(_m.nfacets() * 3);

		const int size = _m.nfacets();
		for (int fi = 0; fi < size; ++fi) {
			// Compute bary
			const int off = fi * nvertsPerFacet;
			vec3 b(0, 0, 0);

			for (int lv = 0; lv < nvertsPerFacet; ++lv) {
				b += _m.points[_m.facets[off + lv]];
			}

			barys[fi * 3] = b.x / nvertsPerFacet;
			barys[fi * 3 + 1] = b.y / nvertsPerFacet;
			barys[fi * 3 + 2] = b.z / nvertsPerFacet;
		}

		glBindBuffer(GL_TEXTURE_BUFFER, bufBary);
		glBufferData(GL_TEXTURE_BUFFER, barys.size() * sizeof(float), barys.data(), GL_STATIC_DRAW);

		// Get selected attribute => 
		// pushAttrs();
		// pushHighlights();
		// pushFilters();
	}

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

	// TODO not tested !
	void setFilter(std::vector<bool> filters) {
		std::vector<float> f_filters(filters.size());
		std::transform(filters.begin(), filters.end(), f_filters.begin(), [](bool filter) { return filter ? 1.f : 0.f; });
		std::memcpy(ptrFilter, f_filters.data(), f_filters.size() * sizeof(float));
	}

	float* &getFilterPtr() {
		return ptrFilter;
	}

	void setAttribute(std::vector<float> attributeData);
	void setAttribute(ContainerBase *ga, int element);

	MeshShader shader;

	protected:

	Surface &_m;

	bool visible = true;

	unsigned int VAO, VBO; // Buffers
	unsigned int bufBary, bufHighlight, bufAttr, bufFilter; // Sample buffers
	unsigned int texColorMap, texBary, texHighlight, texAttr, texFilter; // Textures

	float *ptrAttr;
	float *ptrHighlight;
	float *ptrFilter;

	int nverts = 0;
};