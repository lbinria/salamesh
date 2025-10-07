#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

#include "../include/json.hpp"
using json = nlohmann::json;

#include "renderer.h"
#include "model.h"
#include "element.h"
#include "vertex.h"
using namespace UM;

struct SurfaceRenderer : public IRenderer {
	
	struct Vertex {
		int vertexIndex;
		int localIndex;
		glm::vec3 p;
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
		glm::vec3 p3;
		float size;
		glm::vec3 normal; 
		glm::vec3 heights;
		int facetIndex;
		int cellIndex;
	};

	SurfaceRenderer(Surface &m) : 
		IRenderer(Shader("shaders/surface.vert", "shaders/surface.frag")),
		_m(m)
		{
			shader.use();
			shader.setFloat3("color", {0.8f, 0.f, 0.2f});
		}


	void init();
	void render(glm::vec3 &position);
	void clean();

	virtual void push() override = 0;

	int getRenderElementKind() override { return ElementKind::FACETS; }


	protected:

	Surface &_m;

	void push_bary(std::vector<Vertex> &vertices) {

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
	}

	private:

	void doLoadState(json &j) override {}
	void doSaveState(json &j) const override {}
};