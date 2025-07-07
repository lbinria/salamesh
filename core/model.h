#pragma once

#include "element.h"

#include "../include/glm/glm.hpp"

#include <ultimaille/all.h>
using namespace UM;

struct Model {

    // Color modes:
    // Solid color display, or attribute display
    enum ColorMode {
        COLOR,
        ATTRIBUTE,
    };

    // TODO maybe useless (because using Element for picking) maybe merge with color mode ?
    enum RenderMode {
        Color = 0,
        Pick_facet = 2,
        Pick_cell = 3
    };

	template<typename T>
	T& as() {
		return static_cast<T&>(*this);
	}

	virtual void load(const std::string path) = 0;

    virtual std::string getName() = 0;
    virtual void setName(std::string name) = 0;

    static constexpr const char* colorModeStrings[2] = {"Color", "Attribute"};
    constexpr std::array<std::string_view, 2> getColorModeStrings() {
        return {colorModeStrings[0], colorModeStrings[1]};
    }

    // Lifecycle functions
    virtual void init() = 0; // TODO see because it is an app function => it should be override by user but not used ! (public for override, private for use ?)
    virtual void push() = 0;
    virtual void bind() = 0; // TODO see because it is an app function => it should be override by user but not used ! (public for override, private for use ?)
    virtual void render() = 0;
    virtual void clean() = 0;

	// TODO temp for refactoring
	virtual Hexahedra& getHexahedra() = 0;

    // General shader uniforms
	virtual void setHighlight(int idx, bool highlighted) = 0;
	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;

    virtual int getColorMode() = 0;
    virtual void setColorMode(ColorMode mode) = 0;

    virtual bool getLight() = 0;
    virtual void setLight(bool enabled) = 0;
    virtual bool getLightFollowView() = 0;
    virtual void setLightFollowView(bool follow) = 0;
    virtual bool getClipping() = 0;
    virtual void setClipping(bool enabled) = 0;
    virtual void setFilter(int idx, bool filter) = 0;
    virtual glm::vec3 getClippingPlanePoint() = 0;
    virtual void setClippingPlanePoint(glm::vec3 p) = 0;
    virtual glm::vec3 getClippingPlaneNormal() = 0;
    virtual void setClippingPlaneNormal(glm::vec3 n) = 0;
    virtual void setInvertClipping(bool invert) = 0;
    virtual bool getInvertClipping() = 0;
    virtual float getMeshSize() = 0;
    virtual void setMeshSize(float val) = 0;
    virtual float getMeshShrink() = 0;
    virtual void setMeshShrink(float val) = 0;
    virtual int getFragRenderMode() = 0;
    virtual void setFragRenderMode(RenderMode mode) = 0;

    // Model attributes
    virtual std::vector<std::tuple<std::string, int>> getAttrs() = 0;
    virtual std::tuple<std::string, int> getAttr(int idx) = 0;
    virtual void addAttr(Element element, NamedContainer &container) = 0;
    virtual void removeAttr(const std::string& name, Element element) = 0;
    virtual void clearAttrs() = 0;

    // Model attributes shader uniforms
    virtual void setSelectedAttr(int idx) = 0;
    virtual int getSelectedAttr() const = 0;
    virtual int getSelectedColormap() const = 0;
    virtual void setSelectedColormap(int idx) = 0;

    // Model (maybe not virtual !)
    virtual glm::vec3 getPosition() = 0;
    virtual void setPosition(glm::vec3 p) = 0;

    virtual void setView(glm::mat4 &view) = 0;
    virtual void setProjection(glm::mat4 &projection) = 0;

};