#pragma once

#include <vector>
#include <optional>

#include "../include/json.hpp"
using json = nlohmann::json;

#include <ultimaille/all.h>

#include "../color_mode.h"

#include "renderer.h"

#include "../shader.h"

using namespace UM;

struct PointSetRenderer : public IRenderer {

	struct Vertex {
		int vertexIndex;
		glm::vec3 position;
		float size;
		// glm::vec3 normal;
	};

    PointSetRenderer(PointSet &ps) : 
        IRenderer(Shader("shaders/point.vert", "shaders/point.frag")),
        ps(ps) {
            setPointSize(4.0f); // TODO here use a setting default point size
            setColor({0.23, 0.85, 0.66}); // TODO here use a setting default point color
        }

    void init();
    void push();
    void render(glm::vec3 &position);
    void clean();

	int getRenderElementKind() override { return ElementKind::POINTS_ELT; }

    float getPointSize() const {
        return pointSize;
    }

    void setPointSize(float size) {
        shader.use();
        shader.setFloat("pointSize", size);
        pointSize = size;
    }

    glm::vec3 getColor() const {
        return color;
    }
    
    void setColor(glm::vec3 c) {
        shader.use();
        shader.setFloat3("pointColor", c);
        color = c;
    }


    PointSet &ps;
    // Surface *_m = nullptr;


    private:
    
    std::vector<glm::vec3> normals;

    float pointSize;
    glm::vec3 color;

    void doLoadState(json &j) override {
        setPointSize(j["pointSize"].get<float>());
        setColor({j["pointColor"][0].get<float>(), j["pointColor"][1].get<float>(), j["pointColor"][2].get<float>()});
    }

    void doSaveState(json &j) const override {
        j["pointSize"] = pointSize;
        j["pointColor"] = json::array({color.x, color.y, color.z});
    }

};