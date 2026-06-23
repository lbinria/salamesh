#pragma once

#include "element_type.h"
#include "layer.h"

#include "shader.h"
#include "attribute.h"

#include "material_params.h"

#include "glm/glm.hpp"

#include "json.hpp"
using json = nlohmann::json;

#include "helpers.h"

#include "geometry_buffer.h"
#include "material.h"
#include "geometry.h"

struct ShaderBase {



	ShaderBase (const ShaderBase&) = delete;
	ShaderBase& operator= (const ShaderBase&) = delete;

	ShaderBase(std::string name, Shader shader) : 
		name(name.empty() ? sl::generateGuid() : name),
		shader(std::move(shader))
	{}

	template<typename T>
	T& as() {
		static_assert(std::is_base_of_v<ShaderBase, T>, "Renderer::as() can only be used with derived classes of Renderer");
		auto &x = static_cast<T&>(*this);
		return x;
	}

	virtual bool isCompatible(Geometry &geometry) { return false; }

	virtual GeometryBuffer createGeometryBuffer() {
		unsigned int vao, vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		return GeometryBuffer(shader, vao, vbo);
	};

	virtual Material createMaterial() {
		return Material();
	}

	virtual void update(GeometryBuffer &geometryBuffer, Geometry &geometry) {

	}

	virtual unsigned int renderElement() {
		return GL_POINTS;
	}

	virtual void clean() = 0;
	virtual void clear() = 0;

	void loadState(json &j) {
		doLoadState(j);
	}

	void saveState(json &j) const {
		doSaveState(j);
	}

	std::string getName() { return name; }


	Shader &getShader() { return shader; }

	protected:
	Shader shader;

	private:

	std::string name;

	virtual void doLoadState(json &j) = 0;
	virtual void doSaveState(json &j) const = 0;


};