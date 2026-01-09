#pragma once 
#include <string>
#include <ultimaille/all.h>

enum ModelType {
	POINTSET_MODEL = 0,
	POLYLINE_MODEL,
	TRI_MODEL,
	QUAD_MODEL,
	POLYGON_MODEL,
	TET_MODEL,
	HEX_MODEL,
	PYRAMID_MODEL,
	PRISM_MODEL,
	HYBRID_MODEL,
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
	VEC2_ELT,
	VEC3_ELT
};

static std::string modelTypeToString(ModelType t) {
	switch (t) {
		case ModelType::POINTSET_MODEL: return "pointset";
		case ModelType::POLYLINE_MODEL: return "polyline";
		case ModelType::TRI_MODEL: return "tri";
		case ModelType::QUAD_MODEL: return "quad";
		case ModelType::POLYGON_MODEL: return "polygon";
		case ModelType::TET_MODEL: return "tet";
		case ModelType::HEX_MODEL: return "hex";
		case ModelType::PYRAMID_MODEL: return "pyramid";
		case ModelType::PRISM_MODEL: return "prism";
		case ModelType::HYBRID_MODEL: return "hybrid";
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
		case ElementType::VEC3_ELT: return "vec3";
		default: return "unknown";
	}
}

// TODO maybe move elsewhere because it is dependent of ultimaille this is a "glue"
template<typename TMesh>
static ModelType modelTypeFromMeshType() {
	if constexpr (std::is_same_v<TMesh, UM::PointSet>)
		return ModelType::POINTSET_MODEL;
	else if constexpr (std::is_same_v<TMesh, UM::PolyLine>)
		return ModelType::POLYLINE_MODEL;
	else if constexpr (std::is_same_v<TMesh, UM::Triangles>)
		return ModelType::TRI_MODEL;
	else if constexpr (std::is_same_v<TMesh, UM::Quads>)
		return ModelType::QUAD_MODEL;
	else if constexpr (std::is_same_v<TMesh, UM::Polygons>)
		return ModelType::POLYGON_MODEL;
	else if constexpr (std::is_same_v<TMesh, UM::Tetrahedra>)
		return ModelType::TRI_MODEL;
	else if constexpr (std::is_same_v<TMesh, UM::Hexahedra>)
		return ModelType::TRI_MODEL;
	else if constexpr (std::is_same_v<TMesh, UM::Pyramids>)
		return ModelType::TRI_MODEL;
	else 
		return ModelType::HYBRID_MODEL;
}