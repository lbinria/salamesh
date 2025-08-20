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

	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;
	virtual void setFilter(int idx, bool filter) = 0;
	// TODO virtual void setFilter(std::vector<float> filters) = 0;
	virtual void setAttribute(std::vector<float> attributeData) = 0;
	virtual void setAttribute(ContainerBase *ga, int element) = 0;

	virtual float* &getFilterPtr() = 0;
	// virtual void setMeshIndex(int index) = 0;

};