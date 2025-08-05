#pragma once 
#include "element.h"

#include <ultimaille/all.h>

#include <vector>
#include <string>
#include <memory>

using namespace UM;

struct Attribute {
	std::string name;
	Element kind;
	ElementType type;
	std::shared_ptr<ContainerBase> ptr;

	std::string getName() const { return name; }
	Element getKind() const { return kind; }
	ElementType getType() const { return type; }
	std::shared_ptr<ContainerBase> getPtr() const { return ptr; }

	// This wrapper enable the modification of the attribute data for Lua
	// In C++ there is no need to use this, just use regular UM Attribute after binding it to the model
	template<typename T>
	struct Container {
		Container(std::shared_ptr<ContainerBase> p) : ptr(std::static_pointer_cast<AttributeContainer<T>>(p)) {}

		T& operator[](int i) { return ptr->data[i]; }
		const T& operator[](int i) const { return ptr->data[i]; }

		std::shared_ptr<AttributeContainer<T>> ptr;
	};

	template<typename T>
	Container<T> getContainer() const {
		return Container<T>(ptr);
	}

};