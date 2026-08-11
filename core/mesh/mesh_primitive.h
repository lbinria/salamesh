#pragma once
#include "vec.h"

struct PointVertex {
	int vertexIndex;
	sl::algebra::vec3 pos;
	sl::algebra::vec3 normal;
	float size;
};

struct EdgeVertex {
	int vertexIndex;
	sl::algebra::vec3 p0;
	sl::algebra::vec3 p1;
	float side;
	float end;
	sl::algebra::vec3 bary;
};

struct TriangleVertex {
	int vertexIndex;
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