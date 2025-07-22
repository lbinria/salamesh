#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "core/model.h"
#include "core/element.h"
#include "shader.h"

// TODO IMPORTANT see to bind mesh directly to the shader via buffermap and pointers

using namespace UM;

struct Vertex {
    glm::vec3 position; 
    float size;
    glm::vec3 normal; 
    glm::vec3 heights;
    int facetIndex;
	int cellIndex;
    int vertexIndex;
};

struct HexRenderer {

    HexRenderer(Hexahedra &hex) : 
        hex(hex),
        shader("shaders/hex.vert", "shaders/hex.frag")
        {
        }


    // TODO eventually merge init / update
    void init();
    void push();
    void render(glm::vec3 &position);
    

    void clean();

    // void test() {
    //     // It works ! nice ! just have to point to attribute in mesh !
    //     // Then when we set the attribute in the mesh, it will be set in the shader
    //     auto x = static_cast<float*>(highlightsPtr);
    //     for (int i = 0; i < _highlights.size(); ++i)
    //         x[i] = 2.f;
    // }

    void setTexture(unsigned int tex) { texColorMap = tex; }

    void setVisible(bool v) {
        visible = v;
    }

    bool getVisible() const {
        return visible;
    }

    void setHighlight(int idx, bool highlighted) {
        _highlights[idx] = highlighted ? 1.f : 0.f;
    	std::memcpy(highlightsPtr, _highlights.data(), _highlights.size() * sizeof(float));
    }

    void setHighlight(int idx, float highlight) {
        _highlights[idx] = highlight;
    	std::memcpy(highlightsPtr, _highlights.data(), _highlights.size() * sizeof(float));
    }

    void setHighlight(std::vector<float> highlights) {
        for (int i = 0; i < highlights.size(); ++i)
            _highlights[i] = highlights[i];

    	std::memcpy(highlightsPtr, _highlights.data(), _highlights.size() * sizeof(float));
    }

    void setFilter(int idx, bool filter) {
        ptrFilter[idx] = filter ? 1.f : 0.f;
    }

    void setAttribute(std::vector<float> attributeData);

    void changeAttribute(GenericAttributeContainer *ga, int element);

    void setColorMode(Model::ColorMode mode) {
        shader.use();
        shader.setInt("colorMode", mode);
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

    Hexahedra &getHexahedra() {
        return hex;
    }

    private:

    Hexahedra &hex;
    Shader shader;

    unsigned int VAO, VBO; // Buffers
    unsigned int bufBary, bufHighlight, bufAttr, bufFilter; // Sample buffers
    unsigned int texColorMap, texBary, texHighlight, texAttr, texFilter; // Textures

    std::vector<Vertex> vertices;
    // Data
    std::vector<float> _barys;
    std::vector<float> _highlights;

    void *highlightsPtr;
    float *ptrFilter;

    bool visible = true;
};