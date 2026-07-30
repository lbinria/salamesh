#pragma once
#include "vec.h"

struct PointPrimitive {
	int id;
	sl::algebra::vec3 pos;
	sl::algebra::vec3 normal;
	float size;
};


// sl::algebra::vec3 p;

// int halfedgeIndex;
// sl::algebra::vec3 P0;
// sl::algebra::vec3 P1;
// float side;
// float end;
// sl::algebra::vec3 bary;
struct EdgePrimitive {
	int id;
	sl::algebra::vec3 p0;
	sl::algebra::vec3 p1;
	float side;
	float end;
	sl::algebra::vec3 bary;
};


struct TrianglePrimitive {
	int id;
	int facetIndex;
	int cellIndex;
	int localIndex;

	int cornerIndex;

	sl::algebra::vec3 p;
	sl::algebra::vec3 p0;
	sl::algebra::vec3 p1;
	sl::algebra::vec3 p2;
	sl::algebra::vec3 bary;
	sl::algebra::vec3 normal;
	int cornerOff;
};