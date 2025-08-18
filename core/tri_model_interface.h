#pragma once
#include "model.h"

struct ITriModel : public Model {

	using Model::Model;

	virtual Triangles& getTriangles() = 0;
	virtual SurfaceAttributes& getSurfaceAttributes() = 0;

};