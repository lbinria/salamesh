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

	IRenderer(std::unique_ptr<Shader> shader) : shaderus(std::move(shader)) {}


	// void setAttribute(ContainerBase *ga, int element) {
	// 	// Set attribute element to shader
	// 	shader.setAttrElement(element);

	// 	// Prepare data
	// 	std::vector<float> converted_attribute_data;

	// 	// Transform data
	// 	if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {

	// 		converted_attribute_data.resize(a->data.size());
	// 		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](double x) { return static_cast<float>(x);});

	// 	} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
			
	// 		converted_attribute_data.resize(a->data.size());
	// 		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

	// 	} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
			
	// 		converted_attribute_data.resize(a->data.size());
	// 		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

	// 	} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(ga)) {

	// 		converted_attribute_data.resize(a->data.size());
	// 		std::transform(a->data.begin(), a->data.end(), converted_attribute_data.begin(), [](auto x) { return static_cast<float>(x);});

	// 	}

	// 	// Set attribute data to shader
	// 	setAttribute(converted_attribute_data);
	// }


	// void setAttribute(std::vector<float> attributeData) {

	// 	// Get bounds (min-max)
	// 	float min = std::numeric_limits<float>::max(); 
	// 	float max = std::numeric_limits<float>::min();
	// 	for (auto x : attributeData) {
	// 		min = std::min(min, x);
	// 		max = std::max(max, x);
	// 	}

	// 	// Update min/max
	// 	shader.use();
	// 	shader.setFloat2("attrRange", glm::vec2(min, max));

	// 	// Update sample
	// 	glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	// 	glBufferData(GL_TEXTURE_BUFFER, attributeData.size() * sizeof(float), attributeData.data(), GL_STATIC_DRAW);
	// }

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

	std::unique_ptr<Shader> shaderus;

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