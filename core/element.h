#pragma once 
#include <string>

enum ElementKind {
	POINTS_ELT = 1,
	CORNERS_ELT = 2,
	EDGES_ELT = 4,
	FACETS_ELT = 8,
	CELLS_ELT = 16,
	CELL_FACETS_ELT = 32,
	CELL_CORNERS_ELT = 64
};

enum ElementType {
	DOUBLE_ELT,
	INT_ELT,
	BOOL_ELT,
	VEC2_ELT
};

static std::string elementKindToString(ElementKind e) {
	switch (e) {
		case ElementKind::POINTS_ELT: return "points";
		case ElementKind::EDGES_ELT: return "edges";
		case ElementKind::FACETS_ELT: return "facets";
		case ElementKind::CELL_FACETS_ELT: return "cell_facets";
		case ElementKind::CORNERS_ELT: return "corners";
		case ElementKind::CELL_CORNERS_ELT: return "cell_corners";
		case ElementKind::CELLS_ELT: return "cells";
		default: return "unknown";
	}
}

static std::string elementTypeToString(ElementType t) {
	switch (t) {
		case ElementType::DOUBLE_ELT: return "double";
		case ElementType::INT_ELT: return "int";
		case ElementType::BOOL_ELT: return "bool";
		case ElementType::VEC2_ELT: return "vec2";
		default: return "unknown";
	}
}