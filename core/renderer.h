#pragma once
#include <ultimaille/all.h>
using namespace UM;

struct Renderer {

    enum ColorMode {
        COLOR,
        ATTRIBUTE,
    };

    static constexpr const char* colorModeStrings[2] = {"Color", "Attribute"};
    constexpr std::array<std::string_view, 2> getColorModeStrings() {
        return {colorModeStrings[0], colorModeStrings[1]};
    }

    virtual void update() = 0;
    virtual void to_gl(Hexahedra &m) = 0;

	virtual void setHighlight(int idx, bool highlighted) = 0;
	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;

    virtual int getColorMode() = 0;
    virtual void setColorMode(ColorMode mode) = 0;

    virtual bool getLight() = 0;
    virtual void setLight(bool enabled) = 0;
    virtual bool getClipping() = 0;
    virtual void setClipping(bool enabled) = 0;
    virtual float getMeshSize() = 0;
    virtual void setMeshSize(float val) = 0;
    virtual float getMeshShrink() = 0;
    virtual void setMeshShrink(float val) = 0;
    virtual void setSelectedAttr(int idx) = 0;
    virtual int getSelectedAttr() const = 0;
    virtual int getSelectedColormap() const = 0;
    virtual void setSelectedColormap(int idx) = 0;

};