#pragma once
#include <ultimaille/all.h>
using namespace UM;

struct Renderer {

    virtual void update() = 0;
    virtual void to_gl(Hexahedra &m) = 0;

	virtual void setHighlight(int idx, bool highlighted) = 0;
	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;
};