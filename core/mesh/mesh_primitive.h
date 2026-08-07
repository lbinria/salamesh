#pragma once
#include "vec.h"

struct PointVertex {
	int id;
	sl::algebra::vec3 pos;
	sl::algebra::vec3 normal;
	float size;
};

struct EdgeVertex {
	int id;
	sl::algebra::vec3 p0;
	sl::algebra::vec3 p1;
	float side;
	float end;
	sl::algebra::vec3 bary;
};

struct TriangleVertex {
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