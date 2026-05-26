#pragma once

#include <ultimaille/all.h>
using namespace UM;


struct Geometry {

	virtual ~Geometry() {}

	void requestUpdate() {
		_dirty = true;
	}

	bool shouldUpdate() const {
		return _dirty;
	}

	void updateDone() {
		_dirty = false;
	}

	private:
	bool _dirty = true;
};

struct SurfaceGeometry : public Geometry {
	std::unique_ptr<Surface> _m;
};

struct TrianglesGeometry : public Geometry {

	// Remove copy constructors, allow moves
	TrianglesGeometry() = default;
	TrianglesGeometry(const TrianglesGeometry&) = delete;
	TrianglesGeometry(TrianglesGeometry&&) = default;
	TrianglesGeometry& operator=(const TrianglesGeometry&) = delete;
	TrianglesGeometry& operator=(TrianglesGeometry&&) = default;

	Triangles _m;
};

struct QuadsGeometry : public Geometry {

	// Remove copy constructors, allow moves
	QuadsGeometry() = default;
	QuadsGeometry(const QuadsGeometry&) = delete;
	QuadsGeometry(QuadsGeometry&&) = default;
	QuadsGeometry& operator=(const QuadsGeometry&) = delete;
	QuadsGeometry& operator=(QuadsGeometry&&) = default;

	Quads _m;
};

struct VolumeGeometry : public Geometry {
	std::unique_ptr<Volume> _m;
};