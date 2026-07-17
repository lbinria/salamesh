#pragma once

#include "element_type.h"
#include "layer.h"

#include "shader.h"
#include "attribute.h"
#include "vertex_container.h"

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

	using Handler = std::function<void(Shader&, Mesh&, MeshBuffer&)>;

	struct PairKey {
		std::type_index shader;
		std::type_index mesh;
		bool operator==(const PairKey& other) const { return shader == other.shader && mesh == other.mesh; }
	};

	struct PairKeyHash {
		std::size_t operator()(const PairKey& k) const {
			return k.shader.hash_code() * 1315423911u ^ k.mesh.hash_code();
		}
	};

	class Dispatcher {
	public:
		template<class ShaderT, class MeshT>
		void registerHandler(std::function<void(ShaderT&, MeshT&, MeshBuffer&)> f) {
			PairKey key{ typeid(ShaderT), typeid(MeshT) };
			handlers[key] = [f](Shader& s, Mesh& m, MeshBuffer& b) {
				f(static_cast<ShaderT&>(s), static_cast<MeshT&>(m), b);
			};
		}

		void update(Shader& shader, Mesh& mesh, MeshBuffer& buffer) {
			PairKey key{ typeid(shader), typeid(mesh) };
			auto it = handlers.find(key);
			if (it != handlers.end()) {
				it->second(shader, mesh, buffer);
				return;
			}

			// Optional: fallback strategy if exact pair not registered.
			// e.g. try base classes, or throw, or do a generic path.
			throw std::runtime_error("No handler registered for this (Shader, Mesh) pair");
		}

	private:
		std::unordered_map<PairKey, Handler, PairKeyHash> handlers;
	};



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

	virtual void update(MeshBuffer &meshBuffer, VertexContainer& vertices) {

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


	protected: 
	Dispatcher _dispatcher;
	private:

	std::string name;

};