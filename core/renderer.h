#pragma once
#include <ultimaille/all.h>
using namespace UM;

struct Renderer {

    virtual void update() = 0;
    virtual void to_gl(Hexahedra &m) = 0;

	virtual void setHighlight(int idx, bool highlighted) = 0;
	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;

    virtual void setColorMode(int colorMode) = 0;
    virtual bool getLight() = 0;
    virtual void setLight(bool enabled) = 0;
    virtual bool getClipping() = 0;
    virtual void setClipping(bool enabled) = 0;
    virtual float getMeshSize() = 0;
    virtual void setMeshSize(float val) = 0;
    virtual float getMeshShrink() = 0;
    virtual void setMeshShrink(float val) = 0;

};