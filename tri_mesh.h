#pragma once

#include <vector>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"

using namespace UM;

struct TriMesh {

    // TriMesh() : shader("shaders/simple.vert", "shaders/simple.frag", "shaders/simple.geom"), position(0,0,0) {}
    TriMesh() : shader("shaders/simple.vert", "shaders/simple.frag"), position(0,0,0) {}

    void init();
    void to_gl(Triangles &m);
    void bind();

    void render();

    void clean();

    Shader shader;
    glm::vec3 position;

    private:
    unsigned int VAO, VBO, EBO;
    std::vector<float> v_vertices;
    std::vector<int> v_indices;

};