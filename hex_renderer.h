#pragma once

#include <vector>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "core/renderer.h"
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
    int edgeIndex;
};

enum RenderMode {
    Color = 0,
    Pick_facet = 2,
    Pick_cell = 3
};

struct HexRenderer : public Renderer {

    HexRenderer(Shader &shader) : 
        shader(std::move(shader)),
        // shader("shaders/hex.vert", "shaders/hex.frag"), 
        // shader("shaders/point.vert", "shaders/point.frag"), 
        // shader("shaders/point2.vert", "shaders/point2.frag"), 
        position(0,0,0)/*,*/
        {
        }


    // TODO eventually merge init / update
    void update();
    void init(Hexahedra &m);
    void to_gl(Hexahedra &m);
    void render();
    

    void bind();
    void clean();

    Shader shader;
    glm::vec3 position;

    void test() {
        // It works ! nice ! just have to point to attribute in mesh !
        // Then when we set the attribute in the mesh, it will be set in the shader
        auto x = static_cast<float*>(highlightsPtr);
        for (int i = 0; i < _highlights.size(); ++i)
            x[i] = 2.f;
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

    // TODO not tested !
    void setFilter(std::vector<bool> filter) {
        // for (int i = 0; i < filter.size(); ++i)
        //     _filters[i] = filter[i] ? 1.f : 0.f;

    	// std::memcpy(filtersPtr, _filters.data(), _filters.size() * sizeof(float));
        setRange(filter, _filters, filtersPtr);
    }


    // TODO make private
    inline void setRange(std::vector<bool> data, std::vector<float> target, void* ptr) {
        for (int i = 0; i < data.size(); ++i)
            target[i] = data[i] ? 1.f : 0.f;

    	std::memcpy(ptr, target.data(), target.size() * sizeof(float));
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

    void setColorMode(int colorMode) {
        shader.use();
        shader.setInt("colorMode", colorMode);
    }

    bool getLight() {
        return isLightEnabled;
    }

    void setLight(bool enabled) {
        shader.use();
        shader.setFloat("is_light_enabled", enabled);
        isLightEnabled = enabled;
    }

    bool getClipping() {
        return isClipping;
    }

    void setClipping(bool enabled) {
        shader.use();
        shader.setInt("is_clipping_enabled", enabled);
        isClipping = enabled;
    }

    float getMeshSize() {
        return meshSize;
    }

    void setMeshSize(float val) {
        shader.use();
        shader.setFloat("meshSize", val);
        meshSize = val;
    }

    float getMeshShrink() {
        return meshShrink;
    }

    void setMeshShrink(float val) {
        shader.use();
        shader.setFloat("meshShrink", val);
        meshShrink = val;
    }

    void setFragRenderMode(RenderMode mode) {
        shader.use();
        shader.setInt("fragRenderMode", mode);
    }

    void setPoint(glm::vec3 p) {
        shader.use();
        shader.setFloat3("point", p);
    }

    private:

    int ncells;
    // Buffers
    unsigned int VAO, VBO, EBO, cellBaryBuffer, cellHighlightBuffer, cellAttributeBuffer, cellFilterBuffer;
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

    bool isLightEnabled = true;
    bool isClipping = false;
    float meshSize = 0.01f;
    float meshShrink = 0.f;

};