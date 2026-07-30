#pragma once

#include "element_type.h"
#include "layer.h"

#include "shader.h"
#include "attribute.h"

#include "material_params.h"

#include "json.hpp"
using json = nlohmann::json;

#include "helpers.h"

#include "mesh_buffer.h"
#include "material.h"
#include "mesh.h"

#include <typeindex>
#include <unordered_map>
#include <functional>

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

	virtual bool isCompatible(Mesh &mesh) { return false; }


	// TODO maybe to abstract
	virtual void update(MeshBuffer &meshBuffer, Mesh &mesh) {

	}

	virtual unsigned int renderElement() {
		return GL_POINTS;
	}

	virtual void clean() {
		shader.clean();
	}


	std::string getName() { return name; }


	Shader &getShader() { return shader; }

	// TODO move to protected
	virtual MeshBuffer createMeshBuffer() = 0;
	virtual Material createMaterial() = 0;

	protected:
	Shader shader;


	private:

	std::string name;

};