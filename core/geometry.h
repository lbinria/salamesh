#pragma once

#include <ultimaille/all.h>
using namespace UM;


struct Geometry {

	virtual ~Geometry() {}

	void requestUpdate() {
		_dirty = true;
	}

	bool isDirty() const {
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

    TrianglesGeometry() = default;
    TrianglesGeometry(const TrianglesGeometry&) = delete;
    TrianglesGeometry(TrianglesGeometry&&) = default;           // Allow moves
    TrianglesGeometry& operator=(const TrianglesGeometry&) = delete;
    TrianglesGeometry& operator=(TrianglesGeometry&&) = default;

	Triangles _m;
};

struct VolumeGeometry : public Geometry {
	std::unique_ptr<Volume> _m;
};