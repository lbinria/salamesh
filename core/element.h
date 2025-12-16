#pragma once 
#include <string>

enum ModelType {
	POINTSET = 0,
	POLYLINE_MESH,
	TRI_MESH,
	QUAD_MESH,
	POLYGON_MESH,
	TET_MESH,
	HEX_MESH,
	PYRAMID_MESH,
	PRISM_MESH,
	HYBRID_MESH,
};

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

static std::string modelTypeToString(ModelType t) {
	switch (t) {
		case ModelType::POINTSET: return "pointset";
		case ModelType::POLYLINE_MESH: return "polyline";
		case ModelType::TRI_MESH: return "tri";
		case ModelType::QUAD_MESH: return "quad";
		case ModelType::POLYGON_MESH: return "polygon";
		case ModelType::TET_MESH: return "tet";
		case ModelType::HEX_MESH: return "hex";
		case ModelType::PYRAMID_MESH: return "pyramid";
		case ModelType::PRISM_MESH: return "prism";
		case ModelType::HYBRID_MESH: return "hybrid";
		default: return "unknown";
	}
}

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