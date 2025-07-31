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
	std::shared_ptr<ContainerBase> ptr;

	std::string getName() const { return name; }
	Element getKind() const { return kind; }
	std::shared_ptr<ContainerBase> getPtr() const { return ptr; }

};