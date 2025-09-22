#pragma once

#include "renderer.h"
// TODO should not inlcude this, because of ColorMode ! create a separate file for ColorMode
#include "model.h"

#include "shader.h"
#include "vertex.h"

#include <vector>
#include <ultimaille/all.h>
#include "../include/glm/glm.hpp"

using namespace UM;

struct HalfedgeRenderer : public IRenderer {

	struct LineVert {
		glm::vec3 P0;
		glm::vec3 P1;
		float side;
		float end;
	};

	HalfedgeRenderer(Volume &v) : 
		IRenderer(Shader("shaders/edge.vert", "shaders/edge.frag")),
		// shader("shaders/edgebary.vert", "shaders/edgebary.frag"), 
		v(v) {
			setEdgeSize(2.0f); // TODO here use a setting default edge size
			setEdgeInsideColor({0.0, 0.97, 0.73}); // TODO here use a setting default edge inside color
			setEdgeOutsideColor({0.0, 0.6, 0.45}); // TODO here use a setting default edge outside color
		}

	void init();
	void push();
	void render(glm::vec3 &position);
	void clean();

	float getEdgeSize() const {
		return edgeSize;
	}

	void setEdgeSize(float size) {
		shader.use();
		shader.setFloat("uThickness", size);
		edgeSize = size;
	}

	glm::vec3 getEdgeInsideColor() const {
		return edgeInsideColor;
	}

	void setEdgeInsideColor(glm::vec3 color) {
		shader.use();
		shader.setFloat3("uColorInside", color);
		edgeInsideColor = color;
	}

	glm::vec3 getEdgeOutsideColor() const {
		return edgeOutsideColor;
	}

	void setEdgeOutsideColor(glm::vec3 color) {
		shader.use();
		shader.setFloat3("uColorOutside", color);
		edgeOutsideColor = color;
	}

	Volume &v;

	private:

	float edgeSize;
	glm::vec3 edgeInsideColor;
	glm::vec3 edgeOutsideColor;


	
};