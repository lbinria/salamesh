#pragma once 
#include <string>

enum ElementKind {
	CORNERS = -1,
	POINTS = 0, // TODO rename to VERTICES
	EDGES = 1,
	FACETS = 2,
	CELLS = 3,
	CELL_FACETS = 4,
	CELL_CORNERS = 5
};

enum ElementType {
	DOUBLE,
	INT,
	BOOL
};

static std::string elementKindToString(ElementKind e) {
	switch (e) {
		case ElementKind::POINTS: return "points";
		case ElementKind::EDGES: return "edges";
		case ElementKind::FACETS: return "facets";
		case ElementKind::CELL_FACETS: return "cell_facets";
		case ElementKind::CORNERS: return "corners";
		case ElementKind::CELL_CORNERS: return "cell_corners";
		case ElementKind::CELLS: return "cells";
		default: return "unknown";
	}
}

static std::string elementTypeToString(ElementType t) {
	switch (t) {
		case ElementType::DOUBLE: return "double";
		case ElementType::INT: return "int";
		case ElementType::BOOL: return "bool";
		default: return "unknown";
	}
}