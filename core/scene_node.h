#pragma once
#include "shader.h"
#include "geometry.h"
#include "shader_buffer.h"
#include "renderers/renderer.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct SceneNode {


	Geometry& getGeometry() {
		return *_geometry;
	}

	void setGeometry(std::unique_ptr<Geometry> geometry) {
		_geometry = std::move(geometry);
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
		return _parent;
	}

	void setParent(std::shared_ptr<SceneNode> parent) {
			_parent = parent;
	}


	glm::vec3 getWorldPosition() const {
		if (_parent) {
			return _parent->getWorldPosition() + position;
		} else {
			return position;
		}
	}

	glm::vec3 position{0,0,0};
	private:
	std::unique_ptr<Geometry> _geometry;
	std::map<std::string, ShaderBuffer> _shaders;
	std::shared_ptr<SceneNode> _parent;

};