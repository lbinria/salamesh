#pragma once
#include "model.h"

struct IQuadModel : public Model {

	using Model::Model;

	virtual Quads& getQuads() = 0;
	virtual SurfaceAttributes& getSurfaceAttributes() = 0;

};