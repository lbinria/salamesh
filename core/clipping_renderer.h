#pragma once

#include "renderer.h"
#include "color_mode.h"

#include "shader.h"
#include "vertex.h"

#include <vector>
#include <ultimaille/all.h>
#include "../include/glm/glm.hpp"

using namespace UM;

struct ClippingRenderer : public IRenderer {

	struct Vertex {
		glm::vec3 p;
	};

	ClippingRenderer(PointSet &ps) : 
		IRenderer(Shader("shaders/clipping.vert", "shaders/clipping.frag")),
		ps(ps) {
			// setColor(glm::vec3(1.0, 1.0, 1.0));
		}

	void init() override;
	void push() override;
	void render(glm::vec3 &position) override;
	void clean() override;

	int getRenderElementKind() override { return 0; }

	// glm::vec3 getColor() const {
	// 	return color;
	// }

	// void setColor(glm::vec3 c) {
	// 	shader.use();
	// 	shader.setFloat3("color", c);
	// 	color = c;
	// }


	private:
	PointSet &ps;
	glm::vec3 color;
	glm::vec3 clipping_plane_point{0.0f, 0.0f, 0.0f}; // A point on the plane
	glm::vec3 clipping_plane_normal{1.0f, 0.0f, 0.0f}; // The normal of the plane

	
};