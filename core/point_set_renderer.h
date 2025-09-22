#pragma once

#include <vector>

#include <ultimaille/all.h>

#include "color_mode.h"
#include "render_mode.h"

#include "renderer.h"

#include "shader.h"
#include "vertex.h"

using namespace UM;

struct PointSetRenderer : public IRenderer {

    PointSetRenderer(PointSet &ps) : 
        IRenderer(Shader("shaders/point2.vert", "shaders/point2.frag")),
        ps(ps) {
            setPointSize(4.0f); // TODO here use a setting default point size
            setColor({0.23, 0.85, 0.66}); // TODO here use a setting default point color
        }

    void init();
    void push();
    void render(glm::vec3 &position);
    void clean();

    float getPointSize() const {
        return pointSize;
    }

    void setPointSize(float size) {
        shader.use();
        shader.setFloat("pointSize", size);
        pointSize = size;
    }

    glm::vec3 getColor() const {
        return pointColor;
    }
    
    void setColor(glm::vec3 color) {
        shader.use();
        shader.setFloat3("pointColor", color);
        pointColor = color;
    }

    PointSet &ps;

    private:

    float pointSize;
    glm::vec3 pointColor;

};