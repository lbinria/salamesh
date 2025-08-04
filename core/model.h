#pragma once

#include "element.h"
#include "attribute.h"

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
    virtual void save() const = 0;
    virtual void save_as(const std::string path) const = 0;
    virtual std::string save_state() = 0;

    virtual std::string getName() const = 0;
    virtual void setName(std::string name) = 0;
    virtual std::string getPath() const = 0;

    static constexpr const char* colorModeStrings[2] = {"Color", "Attribute"};
    constexpr std::array<std::string_view, 2> getColorModeStrings() {
        return {colorModeStrings[0], colorModeStrings[1]};
    }

    // Lifecycle functions
    virtual void init() = 0; // TODO see because it is an app function => it should be override by user but not used ! (public for override, private for use ?)
    virtual void push() = 0;
    virtual void render() = 0;
    virtual void clean() = 0;

	// TODO temp for refactoring
	virtual Hexahedra& getHexahedra() = 0;

    // General shader uniforms
    virtual void setTexture(unsigned int tex) = 0;
    
	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;
    virtual void setFacetHighlight(int idx, float highlight) = 0;
    virtual void setFacetHighlight(std::vector<float> highlights) = 0;
    virtual void setFilter(int idx, bool filter) = 0;

    virtual int getColorMode() const = 0;
    virtual void setColorMode(Model::ColorMode mode) = 0;
    virtual glm::vec3 getColor() const = 0;
    virtual void setColor(glm::vec3 c) = 0;

    virtual bool getLight() const = 0;
    virtual void setLight(bool enabled) = 0;
    virtual bool getLightFollowView() const = 0;
    virtual void setLightFollowView(bool follow) = 0;
    virtual bool getClipping() const = 0;
    virtual void setClipping(bool enabled) = 0;
    virtual glm::vec3 getClippingPlanePoint() const = 0;
    virtual void setClippingPlanePoint(glm::vec3 p) = 0;
    virtual glm::vec3 getClippingPlaneNormal() const = 0;
    virtual void setClippingPlaneNormal(glm::vec3 n) = 0;
    virtual bool getInvertClipping() const = 0;
    virtual void setInvertClipping(bool invert) = 0;
    virtual float getMeshSize() const = 0;
    virtual void setMeshSize(float val) = 0;
    virtual float getMeshShrink() const = 0;
    virtual void setMeshShrink(float val) = 0;
    virtual int getFragRenderMode() const = 0;
    virtual void setFragRenderMode(Model::RenderMode mode) = 0;

    virtual glm::vec3 getPointColor() const = 0;
    virtual void setPointColor(glm::vec3 color) = 0;
    virtual float getPointSize() const = 0;
    virtual void setPointSize(float size) = 0;

    // Model attributes
    virtual std::vector<Attribute> getAttrs() const = 0;
    virtual Attribute getAttr(int idx) const = 0;
    // virtual void removeAttr(const std::string& name, Element element) = 0;
    virtual void clearAttrs() = 0;

    // Model attributes shader uniforms
    virtual void setSelectedAttr(int idx) = 0;
    virtual void setSelectedAttr(std::string name, Element kind) = 0;
    virtual int getSelectedAttr() const = 0;
    virtual int getSelectedColormap() const = 0;
    virtual void setSelectedColormap(int idx) = 0;

    template<typename T>
    Attribute bindAttr(std::string name, GenericAttribute<T> &attr) {

        // Deduce kind of element
        Element kind = Element::POINTS; // Default element type
        switch (attr.kind()) {
            case AttributeBase::TYPE::POINTS:
                kind = Element::POINTS;
                break;
            case AttributeBase::TYPE::EDGES:
                kind = Element::EDGES;
                break;
            case AttributeBase::TYPE::FACETS:
                kind = Element::FACETS;
                break;
            case AttributeBase::TYPE::CORNERS:
                kind = Element::CORNERS;
                break;
            case AttributeBase::TYPE::CELLS:
                kind = Element::CELLS;
                break;
            case AttributeBase::TYPE::CELLFACETS:
                kind = Element::FACETS;
                break;
            case AttributeBase::TYPE::CELLCORNERS:
                kind = Element::CORNERS;
                break;
            default:
                throw std::runtime_error("Unknown attribute type for container: " + name);
        }

        // Deduce type of element from T 
        ElementType type = ElementType::FLOAT; // Default type
        if constexpr (std::is_same_v<T, double>) {
            type = ElementType::DOUBLE;
        } else if constexpr (std::is_same_v<T, int>) {
            type = ElementType::INT;
        } else if constexpr (std::is_same_v<T, bool>) {
            type = ElementType::BOOL;
        } else if constexpr (std::is_same_v<T, float>) {
            type = ElementType::FLOAT;
        } else {
            throw std::runtime_error("Unknown attribute type for container: " + name);
        }

        // Search if attribute already exists
        for (const auto &attr : attrs) {
            if (attr.getName() == name && attr.getKind() == kind) {
                // Check if the kind & type match
                if (attr.getType() != type || attr.getKind() != kind) {
                    throw std::runtime_error("Attribute already exists with different type / kind: " + name + ": " + 
                        elementKindToString(attr.getKind()) + " / " + elementTypeToString(attr.getType()) + 
                        " vs " + elementKindToString(kind) + " / " + elementTypeToString(type));
                }
                return attr;
            }
        }

        Attribute a(name, kind, type, attr.get_ptr());
        attrs.push_back(a);
        return a;
    }

    // Model (maybe not virtual !)
    virtual glm::vec3 getPosition() const = 0;
    virtual void setPosition(glm::vec3 p) = 0;

    virtual bool getVisible() const = 0;
    virtual void setVisible(bool v) = 0;
    virtual bool getPointVisible() const = 0;
    virtual void setPointVisible(bool v) = 0;

    // virtual void addChildren(std::shared_ptr<Model> child) = 0;
    virtual void setParent(std::shared_ptr<Model> parentModel) = 0;
    virtual std::shared_ptr<Model> getParent() const = 0;
    virtual glm::vec3 getWorldPosition() const = 0;

    protected:
    std::vector<Attribute> attrs;

};