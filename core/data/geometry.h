#pragma once 

#include <ultimaille/all.h>
#include "geometry_buffer.h"

#include <vector>

struct Geometry {

	virtual ~Geometry() = default;

};

// struct SurfaceGeometry : Geometry {

// 	SurfaceGeometry() {}

// 	Surface& getSurface() {
// 		return *_m;
// 	}

// 	const Surface& getSurface() const {
// 		return *_m;
// 	}

// 	private:
//  	std::unique_ptr<Surface> _m;

// };

struct SurfaceGeometry : Geometry {
	virtual Surface &getSurface() = 0;
	virtual const Surface &getSurface() const = 0;
};

struct TrianglesGeometry : SurfaceGeometry {

	TrianglesGeometry() = default;

	TrianglesGeometry(const TrianglesGeometry& m) = delete;
	TrianglesGeometry(TrianglesGeometry&& m) = delete;
	TrianglesGeometry& operator=(const TrianglesGeometry& m) = delete;

	Surface& getSurface() override {
		return _m;
	}

	const Surface& getSurface() const override {
		return _m;
	}

	Triangles &getMesh() { return _m; }

	private:
	Triangles _m;
};

struct QuadsGeometry : SurfaceGeometry {

	Surface& getSurface() override {
		return _m;
	}

	const Surface& getSurface() const override {
		return _m;
	}

	Quads &getMesh() { return _m; }

	
	private:
	Quads _m;
};