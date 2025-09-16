#pragma once

#include <vector>

#include <ultimaille/all.h>

// TODO should not inlcude this, because of Model::ColorMode ! create a separate file for ColorMode
#include "model.h"
#include "renderer.h"

#include "shader.h"
#include "vertex.h"

using namespace UM;

struct PointSetRenderer : public IRenderer {

    PointSetRenderer(PointSet &ps) : 
        shader("shaders/point2.vert", "shaders/point2.frag"), 
        ps(ps) {
            setPointSize(4.0f); // TODO here use a setting default point size
            setColor({0.23, 0.85, 0.66}); // TODO here use a setting default point color
        }

    void setAttribute(ContainerBase *ga, int elementKind);
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

    bool getVisible() const {
        return visible;
    }

    void setVisible(bool v) {
        visible = v;
    }

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

	void setLight(bool enabled) {
        // TODO implement
    }

	void setLightFollowView(bool follow) {
        // TODO implement
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

    void setSelectedColormap(int idx) {
        // TODO implement
    }

    float* &getFilterPtr() {
        return ptrFilter;
    }


    void setFilter(int idx, bool filter) {
        ptrFilter[idx] = filter ? 1.f : 0.f;
    }

	// TODO not tested !
	void setFilter(std::vector<bool> filters) {
		std::vector<float> f_filters(filters.size());
		std::transform(filters.begin(), filters.end(), f_filters.begin(), [](bool filter) { return filter ? 1.f : 0.f; });
		std::memcpy(ptrFilter, f_filters.data(), f_filters.size() * sizeof(float));
	}

    void setHighlight(int idx, float val) {
        ptrHighlight[idx] = val;
    }

    void setHighlight(std::vector<float> highlights) {
        std::memcpy(ptrHighlight, highlights.data(), highlights.size() * sizeof(float));
    }

    PointSet &ps;
    Shader shader;

    private:

    float pointSize;
    glm::vec3 pointColor;

};