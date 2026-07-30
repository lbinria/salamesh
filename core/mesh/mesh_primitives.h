#pragma once
#include "vec.h"

struct PointPrimitive {
	int id;
	sl::algebra::vec3 pos;
	sl::algebra::vec3 normal;
	float size;
};

		// int cornerIndex;

		// int cornerIndex;
		// int cornerOff;

		// int localIndex;

struct TrianglePrimitive {
	int id;
	int facetIndex;
	int cellIndex;
	int localIndex;
	sl::algebra::vec3 p;
	sl::algebra::vec3 p0;
	sl::algebra::vec3 p1;
	sl::algebra::vec3 p2;
	sl::algebra::vec3 bary;
	sl::algebra::vec3 normal;

};