#pragma once

#include <vector>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"

using namespace UM;

struct PointSetRenderer {

	struct Vertex {
		glm::vec3 position; 
		float size;
	};

    PointSetRenderer(PointSet &ps) : 
        // shader("shaders/point.vert", "shaders/point.frag"), 
        shader("shaders/point2.vert", "shaders/point2.frag"), 
        ps(ps) {}

    void init();
    void push();
    void render(glm::vec3 &position);

    void bind();
    void clean();

    void setView(glm::mat4 &view) {
        shader.use();
        shader.setMat4("view", view);
    }

    void setProjection(glm::mat4 &projection) {
        shader.use();
        shader.setMat4("projection", projection);
    }

    PointSet &ps;
    Shader shader;

	private:
	
	// Buffers
    unsigned int VAO, VBO;
	std::vector<Vertex> vertices; // TODO not necessary to keep it in memory, should replace by ptr
};