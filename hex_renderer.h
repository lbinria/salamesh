#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "core/renderer.h"
#include "core/model.h"
#include "core/element.h"
#include "shader.h"
#include "vertex.h"

// TODO IMPORTANT see to bind mesh directly to the shader via buffermap and pointers

using namespace UM;



struct HexRenderer : IRenderer {

    HexRenderer(Hexahedra &m) : 
        _m(m),
        shader("shaders/hex.vert", "shaders/hex.frag")
        {
            setColor({0.8f, 0.f, 0.2f}); // TODO here use a setting default point color
        }


    // TODO eventually merge init / update
    void init();
    void push();
    void render(glm::vec3 &position);
    

    void clean();

    void setTexture(unsigned int tex) { texColorMap = tex; }

    void setVisible(bool v) {
        visible = v;
    }

    bool getVisible() const {
        return visible;
    }

    void setHighlight(int idx, float highlight) {
        ptrHighlight[idx] = highlight;
    }

    void setHighlight(std::vector<float> highlights) {
    	std::memcpy(ptrHighlight, highlights.data(), highlights.size() * sizeof(float));
    }

    void setFilter(int idx, bool filter) {
        ptrFilter[idx] = filter ? 1.f : 0.f;
    }

    void setFacetHighlight(int idx, float highlight) {
        ptrFacetHighlight[idx] = highlight;
    }

    void setFacetHighlight(std::vector<float> highlights) {
    	std::memcpy(ptrFacetHighlight, highlights.data(), highlights.size() * sizeof(float));
    }

    void setAttribute(std::vector<float> attributeData);
    void setAttribute(ContainerBase *ga, int element);

    void setColorMode(Model::ColorMode mode) {
        shader.use();
        shader.setInt("colorMode", mode);
    }

    void setColor(glm::vec3 color) {
        shader.use();
        shader.setFloat3("color", color);
    }

    void setLight(bool enabled) {
        shader.use();
        shader.setFloat("is_light_enabled", enabled);
    }

    void setLightFollowView(bool follow) {
        shader.use();
        shader.setInt("is_light_follow_view", follow);
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

    void setMeshSize(float val) {
        shader.use();
        shader.setFloat("meshSize", val);
    }

    void setMeshShrink(float val) {
        shader.use();
        shader.setFloat("meshShrink", val);
    }

    void setFragRenderMode(Model::RenderMode mode) {
        shader.use();
        shader.setInt("fragRenderMode", mode);
    }

    void setSelectedColormap(int idx) {
        shader.use();
        shader.setInt("colormap", idx);
    }

    float* &getFilterPtr() {
        return ptrFilter;
    }

    void setMeshIndex(int index) {
        shader.use();
        shader.setInt("meshIndex", index);
    }

    private:

    Hexahedra &_m;
    Shader shader;

    bool visible = true;

    unsigned int VAO, VBO; // Buffers
    unsigned int bufBary, bufHighlight, bufFacetHighlight, bufAttr, bufFilter; // Sample buffers
    unsigned int texColorMap, texBary, texHighlight, texFacetHighlight, texAttr, texFilter; // Textures

    float *ptrAttr;
    float *ptrHighlight;
    float *ptrFacetHighlight;
    float *ptrFilter;

    int nverts = 0;
};