#pragma once

#include "model.h"
#include "element.h"
#include "../include/glm/glm.hpp"

struct IRenderer {

	virtual void init() = 0;
	virtual void push() = 0;
	virtual void render(glm::vec3 &position) = 0;
	virtual void clean() = 0;
	
	virtual void setTexture(unsigned int tex) = 0;

	virtual bool getVisible() const = 0;
	virtual void setVisible(bool v) = 0;

	virtual void setAttribute(std::vector<float> attributeData) = 0;
	virtual void setAttribute(ContainerBase *ga, int element) = 0;

	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;
	
	virtual void setFilter(int idx, bool filter) = 0;
	virtual void setFilter(std::vector<bool> filters) = 0;

	virtual float* &getFilterPtr() = 0;

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