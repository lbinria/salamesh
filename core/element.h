#pragma once 

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
	FLOAT2,
	DOUBLE2,
	INT2,
	FLOAT3,
	DOUBLE3,
	INT3,
	FLOAT4,
	DOUBLE4,
	INT4
};