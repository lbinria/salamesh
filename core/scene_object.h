#pragma once 

#include "helpers.h"

#include <memory>
#include <string>

#include "../../include/glm/glm.hpp"

struct SceneObject {

	SceneObject (const SceneObject&) = delete;
	SceneObject& operator= (const SceneObject&) = delete;

	SceneObject(std::string name) :  
	 _id(sl::generateGuid()),
	_name(name) {}

	std::shared_ptr<SceneObject> getParent() const {
		return _parent;
	}

	void setParent(std::shared_ptr<SceneObject> parent) {
			_parent = parent;
	}

	glm::vec3 getWorldPosition() const {
		if (_parent) {
			return _parent->getWorldPosition() + _position;
		} else {
			return _position;
		}
	}

	private:
	std::string _id;
	std::string _name;

	glm::vec3 _position{0, 0, 0};
	glm::vec3 _rotation{0, 0, 0};

	std::shared_ptr<SceneObject> _parent;

};