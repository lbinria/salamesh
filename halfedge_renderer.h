#pragma once

#include "core/renderer.h"
// TODO should not inlcude this, because of Model::ColorMode ! create a separate file for ColorMode
#include "core/model.h"

#include "shader.h"
#include "vertex.h"

#include <vector>
#include <ultimaille/all.h>
#include "../include/glm/glm.hpp"

using namespace UM;

struct HalfedgeRenderer : public IRenderer {

	struct LineVert {
		glm::vec3 P0;
		glm::vec3 P1;
		float side;
		float end;
	};

	HalfedgeRenderer(Volume &v) : 
		shader("shaders/edge.vert", "shaders/edge.frag"), 
		// shader("shaders/edgebary.vert", "shaders/edgebary.frag"), 
		v(v) {
			setEdgeSize(2.0f); // TODO here use a setting default edge size
			setEdgeInsideColor({0.0, 0.97, 0.73}); // TODO here use a setting default edge inside color
			setEdgeOutsideColor({0.0, 0.6, 0.45}); // TODO here use a setting default edge outside color
		}

	void setAttribute(ContainerBase *ga, int elementKind);
	void setAttribute(std::vector<float> attributeData);

	void init();
	void push();
	void render(glm::vec3 &position);
	void clean();

	void setColorMode(Model::ColorMode mode) {
		shader.use();
		shader.setInt("colorMode", mode);
	}

	// TODO rename setColorMap
	void setTexture(unsigned int tex) {
		texColorMap = tex;
	}

	bool getVisible() const {
		return visible;
	}

	void setVisible(bool v) {
		visible = v;
	}

	float getEdgeSize() const {
		return edgeSize;
	}

	void setEdgeSize(float size) {
		shader.use();
		shader.setFloat("uThickness", size);
		edgeSize = size;
	}

	glm::vec3 getEdgeInsideColor() const {
		return edgeInsideColor;
	}

	void setEdgeInsideColor(glm::vec3 color) {
		shader.use();
		shader.setFloat3("uColorInside", color);
		edgeInsideColor = color;
	}

	glm::vec3 getEdgeOutsideColor() const {
		return edgeOutsideColor;
	}

	void setEdgeOutsideColor(glm::vec3 color) {
		shader.use();
		shader.setFloat3("uColorOutside", color);
		edgeOutsideColor = color;
	}

	void setFilter(int idx, bool filter) {
		ptrFilter[idx] = filter ? 1.f : 0.f;
	}

	float* &getFilterPtr() {
		return ptrFilter;
	}

	// TODO not tested !
	void setFilter(std::vector<bool> filters) {
		std::vector<float> f_filters(filters.size());
		std::transform(filters.begin(), filters.end(), f_filters.begin(), [](bool filter) { return filter ? 1.f : 0.f; });
		std::memcpy(ptrFilter, f_filters.data(), f_filters.size() * sizeof(float));
	}

	void setHighlight(int idx, float val) {
		ptrHighlight[idx] = val;
	}

	void setHighlight(std::vector<float> highlights) {
		std::memcpy(ptrHighlight, highlights.data(), highlights.size() * sizeof(float));
	}

	Volume &v;
	Shader shader;

	private:

	// Buffers
	unsigned int VAO, VBO, pointHighlightBuffer, bufAttr, bufFilter, bufHighlight;
	// Textures
	unsigned int texColorMap, pointHighlightTexture, texAttr, texFilter, texHighlight;

	float* ptrFilter;
	float* ptrHighlight;

	float edgeSize;
	glm::vec3 edgeInsideColor;
	glm::vec3 edgeOutsideColor;

	bool visible = true;

	int npoints = 0;
};