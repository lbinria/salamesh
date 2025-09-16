#pragma once 
#include "../include/glm/glm.hpp"

struct Vertex {
    int vertexIndex;
    glm::vec3 position; 
    float size;
    glm::vec3 normal; 
    glm::vec3 heights;
    int facetIndex;
	int cellIndex;
};