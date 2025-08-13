#pragma once

#include "element.h"
#include "attribute.h"

#include "../include/glm/glm.hpp"
#include <ultimaille/all.h>
using namespace UM;

#include <optional>

struct Model {

    enum ModelType {
        POINTSET = 0,
        POLYLINE = 1,
        TRI = 2,
        QUAD = 3,
        TET = 4,
        HEX = 5,
        HYBRID = 6 
    };

    // Color modes:
    // Solid color display, or attribute display
    enum ColorMode {
        COLOR,
        ATTRIBUTE,
    };

    // TODO maybe useless (because using ElementKind for picking) maybe merge with color mode ?
    enum RenderMode {
        Color = 0,
        Pick_facet = 2,
        Pick_cell = 3
    };

	template<typename T>
	T& as() {
        static_assert(std::is_base_of_v<Model, T>, "Model::as() can only be used with derived classes of Model");
        // static_assert(getModelType() == T::getModelType(), "Model::as() can only be used with the same ModelType");
		return static_cast<T&>(*this);
	}

    virtual ModelType getModelType() const = 0;

	virtual bool load(const std::string path) = 0;
    virtual void save() const = 0;
    virtual void saveAs(const std::string path) const = 0;
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

	// TODO temp for refactoring because model is not necessarily a hex model
	virtual Hexahedra& getHexahedra() = 0;
	virtual VolumeAttributes& getVolumeAttributes() = 0;

	virtual Triangles& getTriangles() = 0;
	virtual SurfaceAttributes& getSurfaceAttributes() = 0;

    virtual int nverts() const = 0; 
    virtual int nfacets() const = 0; 
    virtual int ncells() const = 0; 

    // General shader uniforms
    virtual void setTexture(unsigned int tex) = 0;
    
	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;
    virtual void setFacetHighlight(int idx, float highlight) = 0;
    virtual void setFacetHighlight(std::vector<float> highlights) = 0;
    virtual void setPointHighlight(int idx, float highlight) = 0;
    virtual void setPointHighlight(std::vector<float> highlights) = 0;
    // TODO setEdgeHighlight

    virtual void setFilter(int idx, bool filter) = 0;
    // TODO setFacetFilter
    // TODO setPointFilter
    // TODO setEdgeFilter

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


    virtual float getEdgeSize() const = 0;
    virtual void setEdgeSize(float size) = 0;
    virtual glm::vec3 getEdgeInsideColor() const  = 0;
    virtual void setEdgeInsideColor(glm::vec3 color)  = 0;
    virtual glm::vec3 getEdgeOutsideColor() const  = 0;
    virtual void setEdgeOutsideColor(glm::vec3 color) = 0;

    // Model attributes
    virtual std::vector<Attribute> getAttrs() const = 0;
    virtual Attribute getAttr(int idx) const = 0;
    // virtual void removeAttr(const std::string& name, ElementKind element) = 0;
    virtual void clearAttrs() = 0;

    // Model attributes shader uniforms
    virtual int getSelectedColormap() const = 0;
    virtual void setSelectedColormap(int idx) = 0;

    virtual void setSelectedAttr(int idx) = 0;
    virtual void setSelectedAttr(std::string name, ElementKind kind) = 0;
    virtual int getSelectedAttr() const = 0;

    template<typename T>
    ElementType deduceType(GenericAttribute<T> &attr) {
        if constexpr (std::is_same_v<T, double>) {
            return ElementType::DOUBLE;
        } else if constexpr (std::is_same_v<T, int>) {
            return ElementType::INT;
        } else if constexpr (std::is_same_v<T, bool>) {
            return ElementType::BOOL;
        } else {
            throw std::runtime_error("Unknown attribute type for container: " + attr.getName());
        }
    }

    std::optional<Attribute> findAttr(std::string name, ElementKind kind) {
        for (const auto &a : attrs) {
            if (a.getName() == name && a.getKind() == kind) {
                return a;
            }
        }
        return std::nullopt;
    }

    ElementKind umAttributeKind2ElementKind(AttributeBase::TYPE kind) {
        switch (kind) {
            case AttributeBase::POINTS: return ElementKind::POINTS;
            case AttributeBase::EDGES: return ElementKind::EDGES;
            case AttributeBase::FACETS: return ElementKind::FACETS;
            case AttributeBase::CORNERS: return ElementKind::CORNERS;
            case AttributeBase::CELLS: return ElementKind::CELLS;
            case AttributeBase::CELLFACETS: return ElementKind::CELL_FACETS;
            case AttributeBase::CELLCORNERS: return ElementKind::CELL_CORNERS;
            default: throw std::runtime_error("Unknown attribute kind for binding: " + std::to_string(kind));
        }
    }

    template<typename T>
    Attribute bindAttr(std::string name, ElementKind kind) {
        switch (kind) {
            case ElementKind::POINTS:
                return bindAttr<T>(name, PointAttribute<T>());
            case ElementKind::EDGES:
                return bindAttr<T>(name, EdgeAttribute<T>());
            case ElementKind::FACETS:
                return bindAttr<T>(name, FacetAttribute<T>());
            case ElementKind::CELL_FACETS:
                return bindAttr<T>(name, CellFacetAttribute<T>());
            case ElementKind::CORNERS:
                return bindAttr<T>(name, CornerAttribute<T>());
            case ElementKind::CELL_CORNERS:
                return bindAttr<T>(name, CellCornerAttribute<T>());
            case ElementKind::CELLS:
                return bindAttr<T>(name, CellAttribute<T>());
            default:
                throw std::runtime_error("Unknown element kind for binding attribute: " + elementKindToString(kind));
        }
    }

    template<typename T>
    Attribute bindAttr(std::string name, GenericAttribute<T> &attr) {

        // Deduce kind of element
        ElementKind kind = umAttributeKind2ElementKind(attr.kind());
        // Deduce type of element from T 
        ElementType type = deduceType(attr);

        // Search if salamesh attribute already exists
        auto opt_attr = findAttr(name, kind);
        if (opt_attr.has_value()) {
            auto &a = opt_attr.value();
            a.ptr = attr.get_ptr();
            return a;
        }

        // Does not exist, create a new attribute
        Attribute a(name, kind, type, attr.get_ptr());
        attrs.push_back(a);
        return a;
    }


    // Model (maybe not virtual !)
    virtual void setMeshIndex(int index) = 0;
    virtual glm::vec3 getPosition() const = 0;
    virtual void setPosition(glm::vec3 p) = 0;

    virtual bool getVisible() const = 0;
    virtual void setVisible(bool v) = 0;
    virtual bool getMeshVisible() const = 0;
    virtual void setMeshVisible(bool v) = 0;    
    virtual bool getPointVisible() const = 0;
    virtual void setPointVisible(bool v) = 0;
    virtual bool getEdgeVisible() const = 0;
    virtual void setEdgeVisible(bool v) = 0;

    virtual void setParent(std::shared_ptr<Model> parentModel) = 0;
    virtual std::shared_ptr<Model> getParent() const = 0;
    virtual glm::vec3 getWorldPosition() const = 0;

    protected:
    std::vector<Attribute> attrs;

};