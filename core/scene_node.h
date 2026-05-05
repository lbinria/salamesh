#pragma once 

#include "helpers.h"
#include "data/geometry.h"
#include "data/material.h"
#include "data/material_buf.h"

#include <memory>
#include <string>

#include "../../include/glm/glm.hpp"

struct SceneNode {

	SceneNode (const SceneNode&) = delete;
	SceneNode& operator= (const SceneNode&) = delete;

	SceneNode(std::string name) :  
	 _id(sl::generateGuid()),
	_name(name) {}

	const std::string getName() const {
		return _name;
	}

	const std::string getId() const {
		return _id;
	}

	std::shared_ptr<SceneNode> getParent() const {
		return _parent;
	}

	void setParent(std::shared_ptr<SceneNode> parent) {
			_parent = parent;
	}

	glm::vec3 getWorldPosition() const {
		if (_parent) {
			return _parent->getWorldPosition() + _position;
		} else {
			return _position;
		}
	}

	// Getter - return const reference
	const std::shared_ptr<Geometry> getGeometry() const {
		return _geometry;
	}

	// Setter - take ownership via move semantics
	void setGeometry(std::shared_ptr<Geometry> geometry) {
		_geometry = std::move(geometry);
	}

	void addMaterial(const std::string& name, std::shared_ptr<Material> material) {
		_materials[name] = material;
	}

	void removeMaterial(const std::string& name) {
		_materials.erase(name);
	}

	const std::shared_ptr<Material>& getMaterial(const std::string& name) const {
		return _materials.at(name);  // Throws std::out_of_range if not found
	}

	// Get mutable material by name - reference
	std::shared_ptr<Material>& getMaterial(const std::string& name) {
		return _materials.at(name);
	}

	const std::map<std::string, std::shared_ptr<Material>>& getMaterials() const {
		return _materials;
	}

	bool hasMaterial(const std::string& name) const {
		return _materials.contains(name);
	}

	// Clear all materials
	void clearMaterials() {
		_materials.clear();
	}

	bool isDirty() const {
		return dirty;
	}

	void cleanDirty() {
		dirty = false;
	}

	private:
	std::string _id;
	std::string _name;
	bool dirty = true;

	glm::vec3 _position{0, 0, 0};
	glm::vec3 _rotation{0, 0, 0};

	std::shared_ptr<SceneNode> _parent;

	std::shared_ptr<Geometry> _geometry;
	std::map<std::string, std::shared_ptr<Material>> _materials;

};