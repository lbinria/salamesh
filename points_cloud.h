#pragma once

#include <vector>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"

using namespace UM;

struct PointsCloud {

	struct Vertex {
		glm::vec3 position; 
		float size;
	};

    PointsCloud(PointSet &ps) : 
        // shader("../shaders/point.vert", "../shaders/point.frag"), 
        shader("../shaders/point2.vert", "../shaders/point2.frag"), 
        position(0,0,0),
        // attributes{attributes},
        ps(ps) {}

    void init();
    void to_gl();
    void render();

    void bind();
    void clean();

    PointSet &ps;
    Shader shader;
    glm::vec3 position;

	private:
	
	// Buffers
    unsigned int VAO, VBO;
	std::vector<Vertex> vertices;
};