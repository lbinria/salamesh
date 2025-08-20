#pragma once

// #include <vector>
// #include <tuple>
// #include <memory>

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "core/model.h"
// #include "core/element.h"
#include "shader.h"

using namespace UM;

struct MeshShader : Shader {
	
	using Shader::Shader;

	// void init();
	using Shader::clean;

	using Shader::use;
	using Shader::setBool;
	using Shader::setFloat;
	using Shader::setFloat2;
	using Shader::setFloat3;
	using Shader::setInt;
	using Shader::setMat4;




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