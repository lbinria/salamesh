#pragma once

#include "model.h"
#include "element.h"
#include "shader.h"
#include "../include/glm/glm.hpp"


struct IRenderer {

	virtual void init() = 0;
	virtual void push() = 0;
	virtual void render(glm::vec3 &position) = 0;
	virtual void clean() = 0;

	void setTexture(unsigned int tex) { texColorMap = tex; }

	void setVisible(bool v) {
		visible = v;
	}

	bool getVisible() const {
		return visible;
	}

	IRenderer(Shader shader) : shader(std::move(shader)) {}

	// TODO see list of shader function, some are specific to mesh_renderer...
	void setModel(glm::mat4 model) {
		shader.use();
		shader.setMat4("model", model);
	}

	void setAttrElement(int element) {
		shader.use();
		shader.setInt("attrElement", element);
	}

	void setAttrRange(glm::vec2 range) {
		shader.use();
		shader.setFloat2("attrRange", range);
	}

	void setHighlightElement(int element) {
		shader.use();
		shader.setInt("highlightElement", element);
	}

	void setFilterElement(int element) {
		shader.use();
		shader.setInt("filterElement", element);
	}

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

	void setMeshIndex(int index) {
		shader.use();
		shader.setInt("meshIndex", index);
	}


	void setAttribute(ContainerBase *ga, int element) {
		// Set attribute element to shader
		shader.use();
		shader.setInt("attrElement", element);

		// Prepare data
		std::vector<float> converted_attribute_data;

		// Transform data
		if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {

			converted_attribute_data.resize(a->data.size());
			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](double x) { return static_cast<float>(x);});

		} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
			
			converted_attribute_data.resize(a->data.size());
			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
			
			converted_attribute_data.resize(a->data.size());
			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(ga)) {

			converted_attribute_data.resize(a->data.size());
			std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

		}

		// Set attribute data to shader
		setAttribute(converted_attribute_data);
	}

	void setAttribute(std::vector<float> data) {

			// Get bounds (min-max)
		float min = std::numeric_limits<float>::max(); 
		float max = std::numeric_limits<float>::min();
		for (auto x : data) {
			min = std::min(min, x);
			max = std::max(max, x);
		}

		// Update min/max
		shader.use();
		shader.setFloat2("attrRange", glm::vec2(min, max));

		// Update sample
		std::memcpy(ptrAttr, data.data(), data.size() * sizeof(float));
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

	// TODO not tested !
	void setFilter(std::vector<bool> filters) {
		std::vector<float> f_filters(filters.size());
		std::transform(filters.begin(), filters.end(), f_filters.begin(), [](bool filter) { return filter ? 1.f : 0.f; });
		std::memcpy(ptrFilter, f_filters.data(), f_filters.size() * sizeof(float));
	}

	float* &getFilterPtr() {
		return ptrFilter;
	}

	Shader shader;

	protected:

	bool visible = true;

	unsigned int VAO, VBO; // Buffers
	unsigned int bufBary, bufHighlight, bufAttr, bufFilter; // Sample buffers
	unsigned int texColorMap, texBary, texHighlight, texAttr, texFilter; // Textures

	float *ptrAttr;
	float *ptrHighlight;
	float *ptrFilter;

	int nverts = 0;

};