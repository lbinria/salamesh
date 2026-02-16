// #pragma once

// #include <vector>
// #include <tuple>
// #include <memory>

// #include <ultimaille/all.h>

// #include "../../include/glm/glm.hpp"
// #include "../../include/glm/gtc/matrix_transform.hpp"
// #include "../../include/glm/gtc/type_ptr.hpp"

// #include "../include/json.hpp"
// using json = nlohmann::json;

// #include "renderer.h"
// #include "../element.h"
// using namespace UM;

// struct PolylineRenderer : public Renderer {
	
// 	struct LineVert {
// 		int edgeIndex;
// 		glm::vec3 p0;
// 		glm::vec3 p1;
// 		float side;
// 		float end;
// 	};

// 	PolylineRenderer(PolyLine &m) : 
// 		Renderer(Shader(sl::shadersPath("polyline.vert"), sl::shadersPath("edge.frag"))),
// 		_m(m)
// 		{
// 			setThickness(2.0f);
// 			setInsideColor({0.0, 0.97, 0.73});
// 			setOutsideColor({0.0, 0.6, 0.45});
// 		}

// 	void init() override;
// 	void render(glm::vec3 &position) override;
// 	void push() override;
// 	void clear() override;
// 	void clean() override;

// 	int getRenderElementKind() override { return ElementKind::EDGES_ELT; }

// 	float getThickness() const {
// 		return edgeSize;
// 	}

// 	void setThickness(float size) {
// 		shader.use();
// 		shader.setFloat("thickness", size);
// 		edgeSize = size;
// 	}

// 	glm::vec3 getInsideColor() const {
// 		return edgeInsideColor;
// 	}

// 	void setInsideColor(glm::vec3 color) {
// 		shader.use();
// 		shader.setFloat3("uColorInside", color);
// 		edgeInsideColor = color;
// 	}

// 	glm::vec3 getOutsideColor() const {
// 		return edgeOutsideColor;
// 	}

// 	void setOutsideColor(glm::vec3 color) {
// 		shader.use();
// 		shader.setFloat3("uColorOutside", color);
// 		edgeOutsideColor = color;
// 	}


// 	protected:

// 	PolyLine &_m;

// 	private:

// 	float edgeSize;
// 	glm::vec3 edgeInsideColor;
// 	glm::vec3 edgeOutsideColor;

// 	void doLoadState(json &j) override {}
// 	void doSaveState(json &j) const override {}
// };