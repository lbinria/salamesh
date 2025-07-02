#pragma once

#include <vector>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"

using namespace UM;



struct Wireframe {

    struct Vertex {
        glm::vec3 position; 
        float size;
        glm::vec3 normal; 
        glm::vec3 heights;
        int facetIndex;
        int cellIndex;
    };

    Wireframe(Hexahedra &m, VolumeAttributes &attributes) : 
        shader("shaders/hex.vert", "shaders/hex.frag"), 
        position(0,0,0),
        attributes{attributes},
        m(m) {}


    void init();
    void push();
    void render();
    void render2();

    void bind();
    void clean();

    Shader shader;
    glm::vec3 position;

    // Not tested !
    void resetHighlights() {
        // _highlights.resize(ncells, 0.f);
    	// std::memcpy(highlightsPtr, _highlights.data(), _highlights.size() * sizeof(float));
        resetRange(_highlights, highlightsPtr);
    }

    void setHighligth(int idx, bool highlighted) {
        _highlights[idx] = highlighted ? 1.f : 0.f;
    	std::memcpy(highlightsPtr, _highlights.data(), _highlights.size() * sizeof(float));
    }

    void setHighligth(int idx, float highlight) {
        _highlights[idx] = highlight;
    	std::memcpy(highlightsPtr, _highlights.data(), _highlights.size() * sizeof(float));
    }

    // TODO refactor with setRange
    // TODO not tested !
    void setHighlight(std::vector<bool> highlight) {
        for (int i = 0; i < highlight.size(); ++i)
            _highlights[i] = highlight[i] ? 1.f : 0.f;

    	std::memcpy(highlightsPtr, _highlights.data(), _highlights.size() * sizeof(float));
    }

    // Not tested !
    void resetFilters() {
        // _filters.resize(ncells, 0.f);
    	// std::memcpy(filtersPtr, _filters.data(), _filters.size() * sizeof(float));
        resetRange(_filters, filtersPtr);
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

    template<typename T>
    inline void resetRange(std::vector<T> target, void* ptr) {
        target.resize(ncells, 0.f);
    	std::memcpy(ptr, target.data(), target.size() * sizeof(float));
    }

    void setAttribute(std::vector<float> attributeData);

    // Shader options
    void changeAttribute(std::string name, int element);

    void setColorMode(int colorMode) {
        shader.use();
        shader.setInt("colorMode", colorMode);
    }

    void setLight(bool enabled) {
        shader.use();
        shader.setFloat("is_light_enabled", enabled);			
    }

    void setClipping(bool enabled) {
        shader.use();
        shader.setInt("is_clipping_enabled", enabled);
    }

    void setMeshSize(float size) {
        shader.use();
        shader.setFloat("meshSize", size);
    }

    void setMeshShrink(float shrink) {
        shader.use();
        shader.setFloat("meshShrink", shrink);
    }

    Hexahedra &m;
    VolumeAttributes &attributes;
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

};