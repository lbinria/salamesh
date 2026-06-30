#pragma once

#include "element_type.h"
#include "layer.h"

#include "shader.h"
#include "attribute.h"

#include "material_params.h"

#include "json.hpp"
using json = nlohmann::json;

#include "helpers.h"

#include "geometry_buffer.h"
#include "material.h"
#include "geometry.h"
#include "view_component.h"

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

	ViewComponent createViewComponent(const std::string name, Geometry &geometry) {
		auto geometryBuffer = createGeometryBuffer();
		auto material = createMaterial();
		return ViewComponent(name, geometry, geometryBuffer, material, *this);
	}


	// TODO maybe to abstract
	virtual void update(GeometryBuffer &geometryBuffer, Geometry &geometry) {

	}

	virtual unsigned int renderElement() {
		return GL_POINTS;
	}

	virtual void clean() = 0;
	virtual void clear() = 0;



	std::string getName() { return name; }


	Shader &getShader() { return shader; }

	// TODO move to protected
	virtual GeometryBuffer createGeometryBuffer() = 0;
	virtual Material createMaterial() = 0;

	protected:
	Shader shader;



	private:

	std::string name;

};