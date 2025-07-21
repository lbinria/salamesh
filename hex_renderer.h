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
    

    void bind();
    void clean();

    // void test() {
    //     // It works ! nice ! just have to point to attribute in mesh !
    //     // Then when we set the attribute in the mesh, it will be set in the shader
    //     auto x = static_cast<float*>(highlightsPtr);
    //     for (int i = 0; i < _highlights.size(); ++i)
    //         x[i] = 2.f;
    // }

    void setTexture(unsigned int tex);

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
        _filters[idx] = filter ? 1.f : 0.f;
    	std::memcpy(filtersPtr, _filters.data(), _filters.size() * sizeof(float));
    }

    void setAttribute(std::vector<float> attributeData);

    // Shader options
    // TODO can deduce element from type with overloading of function !
    template<typename T>
    void changeAttribute(CellAttribute<T> &a, int element) {
        // Set attribute element to shader
        shader.use();
        shader.setInt("attr_element", element);
        // Transform data
        std::vector<float> converted_attribute_data(a.ptr->data.size());
        std::transform(a.ptr->data.begin(), a.ptr->data.end(), converted_attribute_data.begin(), [](T x) { return static_cast<T>(x);});
        // Set attribute data to shader
        setAttribute(converted_attribute_data);
    }

    void changeAttribute(GenericAttributeContainer *ga, int element) {
        // Set attribute element to shader
        shader.use();
        shader.setInt("attr_element", element);
        // Transform data
        if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {

            std::vector<float> converted_attribute_data(a->data.size());
            std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](double x) { return static_cast<float>(x);});

            // Set attribute data to shader
            setAttribute(converted_attribute_data);
        } else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {

        } else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {

        }
    }

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

    void setView(glm::mat4 &view) {
        shader.use();
        shader.setMat4("view", view);
    }

    void setProjection(glm::mat4 &projection) {
        shader.use();
        shader.setMat4("projection", projection);
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

    // Buffers
    unsigned int VAO, VBO, cellBaryBuffer, cellHighlightBuffer, cellAttributeBuffer, cellFilterBuffer;
    // Textures
    unsigned int cellBaryTexture, cellHighlightTexture, cellAttributeTexture, cellFilterTexture;

    std::vector<Vertex> v_vertices;
    // Data
    std::vector<float> _barys;
    std::vector<float> _highlights;
    std::vector<float> _attributeData;
    std::vector<float> _filters;

    void *highlightsPtr;
    void *filtersPtr;

    bool visible = true;
};