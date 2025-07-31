#pragma once

#include <vector>

#include <ultimaille/all.h>

// TODO should not inlcude this, because of Model::ColorMode ! create a separate file for ColorMode
#include "core/model.h"


#include "shader.h"
#include "vertex.h"

using namespace UM;

struct PointSetRenderer {

    PointSetRenderer(PointSet &ps) : 
        // shader("shaders/point.vert", "shaders/point.frag"), 
        shader("shaders/point2.vert", "shaders/point2.frag"), 
        ps(ps) {
            setPointSize(4.0f); // TODO here use a setting default point size
            setPointColor({0.23, 0.85, 0.66}); // TODO here use a setting default point color
        }

    void setAttribute(ContainerBase *ga);
    void setAttribute(std::vector<float> attributeData);

    void init();
    void push();
    void render(glm::vec3 &position);
    void clean();

    void setColorMode(Model::ColorMode mode) {
        shader.use();
        shader.setInt("colorMode", mode);
    }

    // TODO rename setColorMap
    void setTexture(unsigned int tex) {
        texColorMap = tex;
    }

    void setVisible(bool v) {
        visible = v;
    }

    bool getVisible() const {
        return visible;
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

    void setClipping(bool enabled) {
        shader.use();
        shader.setInt("is_clipping_enabled", enabled);
    }

    void setClippingPlanePoint(glm::vec3 p) {
        shader.use();
        shader.setFloat3("clipping_plane_point", p);
    }

    void setClippingPlaneNormal(glm::vec3 n) {
        shader.use();
        shader.setFloat3("clipping_plane_normal", n);
    }

    void setInvertClipping(bool invert) {
        shader.use();
        shader.setInt("invert_clipping", invert);
    }

    void setFilter(int idx, bool filter) {
        ptrFilter[idx] = filter ? 1.f : 0.f;
    }

    void setHighlight(int idx, float val) {
        ptrHighlight[idx] = val;
    }

    PointSet &ps;
    Shader shader;

	private:
	
	// Buffers
    unsigned int VAO, VBO, pointHighlightBuffer, bufAttr, bufFilter, bufHighlight;
    // Textures
    unsigned int texColorMap, pointHighlightTexture, texAttr, texFilter, texHighlight;

    float* ptrFilter;
    float* ptrHighlight;

    float pointSize;
    glm::vec3 pointColor;

    bool visible = true;
};