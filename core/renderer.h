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
	virtual void setColorMode(Model::ColorMode mode) = 0;
	virtual void setColor(glm::vec3 color) = 0;

	virtual void setLight(bool enabled) = 0;
	virtual void setLightFollowView(bool follow) = 0;

	virtual void setClipping(bool enabled) = 0;
	virtual void setClippingPlanePoint(glm::vec3 p) = 0;
	virtual void setClippingPlaneNormal(glm::vec3 n) = 0;
	virtual void setInvertClipping(bool invert) = 0;
	// virtual void setMeshSize(float val) = 0;
	// virtual void setMeshShrink(float val) = 0;
	// virtual void setFragRenderMode(Model::RenderMode mode) = 0;
	virtual void setSelectedColormap(int idx) = 0;

	virtual float* &getFilterPtr() = 0;
	// virtual void setMeshIndex(int index) = 0;

};