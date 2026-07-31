#pragma once 
#include "element_type.h"
#include "vec.h"
#include "helpers.h"

#include <ultimaille/all.h>
using namespace UM;

#include <vector>
#include <string>
#include <memory>
#include <format>

struct Attribute {
	std::string name;
	ElementKind kind;
	ElementType type;
	std::shared_ptr<ContainerBase> ptr; // TODO maybe remove !
	bool isSplit;
	int dim;

	std::string getName() const { return name; }
	ElementKind getKind() const { return kind; }
	ElementType getType() const { return type; }

	std::string getFullName() const {
		return std::format("{}.{}[{}]{}",
			elementKindToString(kind),
			name,
			elementTypeToString(type),
			getNDims() > 1 ? std::format("[{}]", dim) : "");
	}

	bool operator==(const Attribute& other) const {
		return name == other.name &&
				kind == other.kind &&
				type == other.type &&
				dim == other.dim;
	}

	bool operator!=(const Attribute& other) const {
		return !(*this == other);
	}

	// std::shared_ptr<ContainerBase> getPtr() const { return ptr; }

	// This wrapper enable the modification of the attribute data for Lua
	// In C++ there is no need to use this, just use regular UM Attribute after binding it to the model
	template<typename T>
	struct Container {
		Container(std::shared_ptr<ContainerBase> p) : ptr(std::static_pointer_cast<AttributeContainer<T>>(p)) {}

		// T& operator[](int i) { return ptr->data[i]; }
		// const T& operator[](int i) const { return ptr->data[i]; }
		T get(int i) { return ptr->data[i]; }
		void set(int i, T val) { ptr->data[i] = val; }

		std::shared_ptr<AttributeContainer<T>> ptr;
	};

	template<typename T>
	Container<T> getContainer() const {
		return Container<T>(ptr);
	}

	int getNDims() const {
		switch (type)
		{
		case ElementType::VEC3_ELT:
			return 3;
		case ElementType::VEC2_ELT:
			return 2;
		default:
			return 1;
		}
	}

	sl::algebra::vec2 getRange() {
		auto data = sl::getContainerData(ptr.get(), dim);
		auto [min, max] = sl::getRange(data);
		return {min, max};
	}

};