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

struct SurfaceRenderer : public Renderer {
	
	struct Vertex {
		// int vertexIndex;
		int localIndex;
		int cornerIndex;
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
		int facetIndex;
	};

	SurfaceRenderer(Surface &m) : 
		Renderer(Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{
			shader.use();
			shader.setFloat3("color", {0.71f, 0.71f, 0.71f});
		}

	void init() override;
	void render(glm::vec3 &position) override;
	virtual void push() override = 0;
	void clear() override;
	void clean() override;

	int getRenderElementKind() override { return ElementKind::FACETS_ELT | ElementKind::CORNERS_ELT; }


	protected:

	Surface &_m;

	// TODO move to Renderer
	void push(std::vector<Vertex> &vertices) {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	}

	private:

	void doLoadState(json &j) override {}
	void doSaveState(json &j) const override {}
};