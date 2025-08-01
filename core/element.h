#pragma once 
#include <string>

enum Element {
	CORNERS = -1,
	POINTS = 0, // TODO rename to VERTICES
	EDGES = 1,
	FACETS = 2,
	CELLS = 3
};

enum ElementType {
	FLOAT,
	DOUBLE,
	INT,
	BOOL
};

static std::string elementKindToString(Element e) {
	switch (e) {
		case Element::POINTS: return "points";
		case Element::EDGES: return "edges";
		case Element::FACETS: return "facets";
		case Element::CORNERS: return "corners";
		case Element::CELLS: return "cells";
		default: return "unknown";
	}
}

static std::string elementTypeToString(ElementType t) {
	switch (t) {
		case ElementType::FLOAT: return "float";
		case ElementType::DOUBLE: return "double";
		case ElementType::INT: return "int";
		case ElementType::BOOL: return "bool";
		default: return "unknown";
	}
}