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
		int facetIndex;
	};

	// Test
	struct Vertex2 {
		int vertexIndex;
		int facetIndex;
		glm::vec3 p;
		glm::vec3 b;
		glm::vec3 h;
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

	void push(std::vector<Vertex> &vertices) {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	}

	private:

	void doLoadState(json &j) override {}
	void doSaveState(json &j) const override {}
};