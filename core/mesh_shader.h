#pragma once

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

#include "model.h"
#include "shader.h"

using namespace UM;

struct MeshShader : Shader {
	
	using Shader::Shader;

	// using Shader::clean;

	// using Shader::use;
	// using Shader::setBool;
	// using Shader::setFloat;
	// using Shader::setFloat2;
	// using Shader::setFloat3;
	// using Shader::setInt;
	// using Shader::setMat4;

	void setModel(glm::mat4 model) {
		use();
		setMat4("model", model);
	}

	void setAttrElement(int element) {
		use();
		setInt("attrElement", element);
	}

	void setAttrRange(glm::vec2 range) {
		use();
		setFloat2("attrRange", range);
	}

	void setHighlightElement(int element) {
		use();
		setInt("highlightElement", element);
	}

	void setFilterElement(int element) {
		use();
		setInt("filterElement", element);
	}

	void setColorMode(Model::ColorMode mode) {
		use();
		setInt("colorMode", mode);
	}

	void setColor(glm::vec3 color) {
		use();
		setFloat3("color", color);
	}

	void setLight(bool enabled) {
		use();
		setFloat("is_light_enabled", enabled);
	}

	void setLightFollowView(bool follow) {
		use();
		setInt("is_light_follow_view", follow);
	}

	void setClipping(bool enabled) {
		use();
		setInt("is_clipping_enabled", enabled);
	}

	void setClippingPlanePoint(glm::vec3 p) {
		use();
		setFloat3("clipping_plane_point", p);
	}

	void setClippingPlaneNormal(glm::vec3 n) {
		use();
		setFloat3("clipping_plane_normal", n);
	}

	void setInvertClipping(bool invert) {
		use();
		setInt("invert_clipping", invert);
	}

	void setMeshSize(float val) {
		use();
		setFloat("meshSize", val);
	}

	void setMeshShrink(float val) {
		use();
		setFloat("meshShrink", val);
	}

	void setFragRenderMode(Model::RenderMode mode) {
		use();
		setInt("fragRenderMode", mode);
	}

	void setSelectedColormap(int idx) {
		use();
		setInt("colormap", idx);
	}

	void setMeshIndex(int index) {
		use();
		setInt("meshIndex", index);
	}
};