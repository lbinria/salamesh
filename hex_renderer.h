#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "core/renderer.h"
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

// TODO maybe useless (because using Element for picking)
enum RenderMode {
    Color = 0,
    Pick_facet = 2,
    Pick_cell = 3
};

struct HexRenderer : public Renderer {

    HexRenderer(Shader &shader) : 
        shader(std::move(shader)),
        position(0,0,0)
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

    // template<typename T>
    // void changeAttribute(AttributeContainer<T> &a, int element) {
    //     // Set attribute element to shader
    //     shader.use();
    //     shader.setInt("attr_element", element);
    //     // Transform data
    //     std::vector<float> converted_attribute_data(a.data.size());
    //     std::transform(a.data.begin(), a.data.end(), converted_attribute_data.begin(), [](T x) { return static_cast<float>(x);});
    //     // Set attribute data to shader
    //     setAttribute(converted_attribute_data);
    // }

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

    int getColorMode() final override {
        return colorMode;
    }

    void setColorMode(ColorMode mode) final override {
        shader.use();
        shader.setInt("colorMode", mode);
        colorMode = mode;
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

    glm::vec3 getClippingPlanePoint() {
        return clippingPlanePoint;
    }

    void setClippingPlanePoint(glm::vec3 p) {
        shader.use();
        shader.setFloat3("clipping_plane_point", p);
        clippingPlanePoint = p;
    }

    glm::vec3 getClippingPlaneNormal() {
        return clippingPlaneNormal;
    }

    void setClippingPlaneNormal(glm::vec3 n) {
        shader.use();
        shader.setFloat3("clipping_plane_normal", n);
        clippingPlaneNormal = n;
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

    // std::vector<std::tuple<std::string, Element>> getAttrs() {
    //     std::vector<std::tuple<std::string, Element>> result;
    //     for (const auto& attr : attrs) {
    //         result.emplace_back(std::get<0>(attr), std::get<1>(attr));
    //     }
    //     return result;
    // }

    std::vector<std::tuple<std::string, int>> getAttrs() final override {
        std::vector<std::tuple<std::string, int>> result;
        // std::vector<std::pair<std::string, int>> result;
        for (const auto& attr : attrs) {
            result.emplace_back(std::get<0>(attr), std::get<1>(attr));
            // result.push_back(std::make_tuple(std::get<0>(attr), std::get<1>(attr)));
        }
        return result;
    }

    std::tuple<std::string, int> getAttr(int idx) final override {
        return std::make_tuple(std::get<0>(attrs[idx - 1]), std::get<1>(attrs[idx - 1]));
    }

    int getSelectedAttr() const {
        return selectedAttr;
    }

    void setSelectedAttr(int idx) {
        selectedAttr = idx;
        int attr_element = std::get<1>(attrs[idx]);
        changeAttribute(std::get<2>(attrs[idx]).get(), attr_element);
    }

    int getSelectedColormap() const {
        return selectedColormap;
    }

    void setSelectedColormap(int idx) {
        selectedColormap = idx;
        shader.use();
        shader.setInt("colormap", selectedColormap);
    }

    std::vector<std::tuple<std::string, Element, std::shared_ptr<GenericAttributeContainer>>> attrs;

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
    glm::vec3 clippingPlanePoint;
    glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
    float meshSize = 0.01f;
    float meshShrink = 0.f;
    ColorMode colorMode = ColorMode::COLOR;
    
    int selectedAttr = 0;
    int selectedColormap = 0;

};