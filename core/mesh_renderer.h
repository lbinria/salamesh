// #pragma once

// #include <vector>
// #include <tuple>
// #include <memory>

// #include <ultimaille/all.h>

// #include "../include/glm/glm.hpp"
// #include "../include/glm/gtc/matrix_transform.hpp"
// #include "../include/glm/gtc/type_ptr.hpp"

// #include "renderer.h"
// #include "model.h"
// #include "element.h"
// #include "vertex.h"
// using namespace UM;

// struct MeshRenderer : public IRenderer {

// 	MeshRenderer() :
// 		shader("shaders/surface.vert", "shaders/surface.frag")
// 		{
// 			shader.setColor({0.8f, 0.f, 0.2f}); // TODO here use a setting default mesh color
// 		}


// 	// virtual void push() override = 0;

// 	void setTexture(unsigned int tex) { texColorMap = tex; }

// 	void setVisible(bool v) {
// 		visible = v;
// 	}

// 	bool getVisible() const {
// 		return visible;
// 	}

// 	void setHighlight(int idx, float highlight) {
// 		ptrHighlight[idx] = highlight;
// 	}

// 	void setHighlight(std::vector<float> highlights) {
// 		std::memcpy(ptrHighlight, highlights.data(), highlights.size() * sizeof(float));
// 	}

// 	void setFilter(int idx, bool filter) {
// 		ptrFilter[idx] = filter ? 1.f : 0.f;
// 	}

// 	// TODO not tested !
// 	void setFilter(std::vector<bool> filters) {
// 		std::vector<float> f_filters(filters.size());
// 		std::transform(filters.begin(), filters.end(), f_filters.begin(), [](bool filter) { return filter ? 1.f : 0.f; });
// 		std::memcpy(ptrFilter, f_filters.data(), f_filters.size() * sizeof(float));
// 	}

// 	float* &getFilterPtr() {
// 		return ptrFilter;
// 	}

// 	void setAttribute(std::vector<float> attributeData);
// 	void setAttribute(ContainerBase *ga, int element);

// 	MeshShader shader;

// 	protected:

// 	Surface &_m;
// };