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
		int index;
        glm::vec3 position; 
		float size;
	};

    PointSetRenderer(PointSet &ps) : 
        // shader("shaders/point.vert", "shaders/point.frag"), 
        shader("shaders/point2.vert", "shaders/point2.frag"), 
        ps(ps) {
            setPointSize(4.0f);
            setPointColor({0.23, 0.85, 0.66});
        }

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

    void setMeshShrink(float val) {
        shader.use();
        shader.setFloat("meshShrink", val);
    }

    float getPointSize() const {
        return pointSize;
    }

    void setPointSize(float size) {
        shader.use();
        shader.setFloat("pointSize", size);
        pointSize = size;
    }

    glm::vec3 getPointColor() const {
        return pointColor;
    }
    
    void setPointColor(glm::vec3 color) {
        shader.use();
        shader.setFloat3("pointColor", color);
        pointColor = color;
    }

    PointSet &ps;
    Shader shader;

	private:
	
	// Buffers
    unsigned int VAO, VBO, cellBaryBuffer, pointHighlightBuffer, pointAttributeBuffer, pointFilterBuffer;
    // Textures
    unsigned int cellBaryTexture, pointHighlightTexture, pointAttributeTexture, pointFilterTexture;

	std::vector<Vertex> vertices; // TODO not necessary to keep it in memory, should replace by ptr

    // Data
    std::vector<float> _barys;

    float pointSize;
    glm::vec3 pointColor;
};