#pragma once
#include "shader.h"
#include "geometry.h"
#include "shader_buffer.h"
#include "renderers/renderer.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct SceneNode : std::enable_shared_from_this<SceneNode> {

	std::tuple<glm::vec3, glm::vec3> bbox() {
		auto [min, max] = _geometry->bbox();

		for (auto child : _children) {
			auto [childMin, childMax] = child->bbox();
			min = glm::min(min, childMin);
			max = glm::max(max, childMax);
		}

		return {min, max};
	}

	Geometry& getGeometry() {
		return *_geometry;
	}

	void setGeometry(std::unique_ptr<Geometry> geometry) {
		_geometry = std::move(geometry);
	}

	template<typename TGeometry>
	TGeometry& createGeometry() {
		_geometry = std::make_unique<TGeometry>();
		return *static_cast<TGeometry*>(_geometry.get());
	}

	// TODO add getShader

	bool addShader(Material &material) {
		if (_shaders.contains(material.getName()))
			return false;
		
		auto shaderBuffer = material.createShaderBuffer();
		_shaders.emplace(material.getName(), std::move(shaderBuffer));
		return true;
	}

	std::map<std::string, ShaderBuffer>& getShaderBuffers() {
		return _shaders;
	}

	std::optional<std::reference_wrapper<ShaderBuffer>> getShaderBuffer(Material &material) {
		return getShaderBuffer(material.getName());
	}

	std::optional<std::reference_wrapper<ShaderBuffer>> getShaderBuffer(const std::string name) {
		if (!_shaders.contains(name))
			return std::nullopt;
		
		return _shaders.at(name);
	}

	std::shared_ptr<SceneNode> getParent() const {
		return _parent.lock();
	}

	void add(std::shared_ptr<SceneNode> child) {
		child->_parent = shared_from_this();
		_children.push_back(child);
	}

	glm::vec3 getWorldPosition() const {
		if (auto p = _parent.lock()) {
			return p->getWorldPosition() + position;
		} else {
			return position;
		}
	}

	glm::vec3 position{0,0,0};
	private:
	std::unique_ptr<Geometry> _geometry;
	std::map<std::string, ShaderBuffer> _shaders;

	
	std::weak_ptr<SceneNode> _parent;
	std::vector<std::shared_ptr<SceneNode>> _children;

};