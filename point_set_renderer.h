#pragma once

#include <vector>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

// TODO should not inlcude this, because of Model::ColorMode ! create a separate file for ColorMode
#include "core/model.h"


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
            setPointSize(4.0f); // TODO here use a setting default point size
            setPointColor({0.23, 0.85, 0.66}); // TODO here use a setting default point color
        }

    void changeAttribute(GenericAttributeContainer *ga);
    void setAttribute(std::vector<float> attributeData);

    void init();
    void push();
    void render(glm::vec3 &position);


    void bind();
    void clean();

    void setColorMode(Model::ColorMode mode) {
        shader.use();
        shader.setInt("colorMode", mode);
    }

    void setTexture(unsigned int tex) {
        texColorMap = tex;
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
    unsigned int VAO, VBO, bufBary, pointHighlightBuffer, bufAttr, pointFilterBuffer;
    // Textures
    unsigned int texColorMap, texBary, pointHighlightTexture, texAttr, pointFilterTexture;

	std::vector<Vertex> vertices; // TODO not necessary to keep it in memory, should replace by ptr

    // Data TODO maybe not necessary to keep it in memory, should replace by ptr
    std::vector<float> _barys;
    std::vector<float> _highlights;

    float pointSize;
    glm::vec3 pointColor;
};