#pragma once

#include "element.h"
#include "attribute.h"
#include "renderer.h"
#include "color_mode.h"
#include "render_mode.h"

#include "../include/glm/glm.hpp"
#include "../include/json.hpp"

#include <ultimaille/all.h>

#include <optional>

using namespace UM;
using json = nlohmann::json;

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

    // // Color modes:
    // // Solid color display, or attribute display
    // enum ColorMode {
    //     COLOR,
    //     ATTRIBUTE,
    // };

    // // TODO maybe useless (because using ElementKind for picking) maybe merge with color mode ?
    // enum RenderMode {
    //     Color = 0,
    //     Pick_facet = 2,
    //     Pick_cell = 3
    // };

    Model() :
    _name(""),
    _path("") {}

    Model(std::string name) : 
    _name(name), 
    _path("") {}

	template<typename T>
	T& as() {
        static_assert(std::is_base_of_v<Model, T>, "Model::as() can only be used with derived classes of Model");
        // static_assert(getModelType() == T::getModelType(), "Model::as() can only be used with the same ModelType");
		return static_cast<T&>(*this);
	}

    virtual ModelType getModelType() const = 0;

	virtual bool load(const std::string path) = 0;
    virtual void saveAs(const std::string path) const = 0;

    void save() const {
        saveAs(_path);
    }


    std::string save_state() const {
        json j;
        j["name"] = _name;
        j["path"] = _path;
        j["position"] = { position.x, position.y, position.z };
        j["color_mode"] = colorMode;
        j["is_light_enabled"] = isLightEnabled;
        j["is_light_follow_view"] = isLightFollowView;
        j["is_clipping"] = isClipping;
        j["clipping_plane_point"] = { clippingPlanePoint.x, clippingPlanePoint.y, clippingPlanePoint.z };
        j["clipping_plane_normal"] = { clippingPlaneNormal.x, clippingPlaneNormal.y, clippingPlaneNormal.z };
        j["invert_clipping"] = invertClipping;
        j["mesh_size"] = meshSize;
        j["mesh_shrink"] = meshShrink;
        j["frag_render_mode"] = fragRenderMode;
        j["selected_colormap"] = selectedColormap;
        j["visible"] = visible;
        return j.dump(4);
    }

    void load_state(json model_state) {
        
        _name = model_state["name"].get<std::string>();
        _path = model_state["path"].get<std::string>();

        position = glm::vec3(
            model_state["position"][0].get<float>(),
            model_state["position"][1].get<float>(),
            model_state["position"][2].get<float>()
        );
        
        setColorMode((ColorMode)model_state["color_mode"].get<int>());

        setLight(model_state["is_light_enabled"].get<bool>());
        setLightFollowView(model_state["is_light_follow_view"].get<bool>());
        setClipping(model_state["is_clipping"].get<bool>());

        setClippingPlanePoint(glm::vec3(
            model_state["clipping_plane_point"][0].get<float>(),
            model_state["clipping_plane_point"][1].get<float>(),
            model_state["clipping_plane_point"][2].get<float>()
        ));

        setClippingPlaneNormal(glm::vec3(
            model_state["clipping_plane_normal"][0].get<float>(),
            model_state["clipping_plane_normal"][1].get<float>(),
            model_state["clipping_plane_normal"][2].get<float>()
        ));

        setInvertClipping(model_state["invert_clipping"].get<bool>());

        setMeshSize(model_state["mesh_size"].get<float>());
        setMeshShrink(model_state["mesh_shrink"].get<float>());
        
        setSelectedColormap(model_state["selected_colormap"].get<int>());
        setVisible(model_state["visible"].get<bool>());

        load(_path);
    }

    std::string getName() const { return _name; }
    void setName(std::string name) { _name = name; }
    std::string getPath() const { return _path; }

    static constexpr const char* colorModeStrings[2] = {"Color", "Attribute"};
    constexpr std::array<std::string_view, 2> getColorModeStrings() {
        return {colorModeStrings[0], colorModeStrings[1]};
    }

    // Lifecycle functions
    virtual void init() = 0; // TODO see because it is an app function => it should be override by user but not used ! (public for override, private for use ?)
    virtual void push() = 0;
    virtual void render() = 0;
    virtual void clean() = 0;


    virtual int nverts() const = 0; 
    virtual int nfacets() const = 0; 
    virtual int ncells() const = 0; 

    // General shader uniforms
    virtual void setTexture(unsigned int tex) = 0;
    
	virtual void setHighlight(int idx, float highlight) = 0;
	virtual void setHighlight(std::vector<float> highlights) = 0;
    virtual void setPointHighlight(int idx, float highlight) = 0;
    virtual void setPointHighlight(std::vector<float> highlights) = 0;
    // TODO setEdgeHighlight

    virtual void setFilter(int idx, bool filter) = 0;
    // TODO setFacetFilter
    // TODO setPointFilter
    // TODO setEdgeFilter

    virtual int getColorMode() const = 0;
    virtual void setColorMode(ColorMode mode) = 0;
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
    virtual void setFragRenderMode(RenderMode mode) = 0;

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

    void addAttr(ElementKind kind, NamedContainer &container) {
        
        // Get the type of the container
        ElementType type = ElementType::DOUBLE; // Default type
        if (auto a = dynamic_cast<AttributeContainer<double>*>(container.ptr.get())) {
            type = ElementType::DOUBLE;
        } else if (auto a = dynamic_cast<AttributeContainer<int>*>(container.ptr.get())) {
            type = ElementType::INT;
        } else if (auto a = dynamic_cast<AttributeContainer<bool>*>(container.ptr.get())) {
            type = ElementType::BOOL;
        } else {
            throw std::runtime_error("Unknown attribute type for container: " + container.name);
        }

        attrs.emplace_back(container.name, kind, type, container.ptr);
    }


    // Model attributes
    std::vector<Attribute> getAttrs() const {
        return attrs;
    }

    Attribute getAttr(int idx) const {
        return attrs[idx];
    }

    void clearAttrs() {
        attrs.clear();
    }

    int getSelectedAttr() const {
        return selectedAttr;
    }

    virtual void setSelectedAttr(int idx) = 0;
    // virtual void setSelectedAttr(std::string name, ElementKind kind) = 0;

    void setSelectedAttr(std::string name, ElementKind kind) {
        // Search attribute by name
        for (int i = 0; i < attrs.size(); ++i) {
            const auto &attr = attrs[i];

            if (attr.getName() == name && attr.getKind() == kind) {
                setSelectedAttr(i);
                return;
            }
        }

        throw std::runtime_error("Attribute not found: " + name);
    }

    
    void updateAttr() {
        setSelectedAttr(selectedAttr);
    }

    // virtual void removeAttr(const std::string& name, ElementKind element) = 0;

    // Model attributes shader uniforms
    virtual int getSelectedColormap() const = 0;
    virtual void setSelectedColormap(int idx) = 0;

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

    // template<typename T>
    // Attribute bindAttr(std::string name, ElementKind kind) {
    //     switch (kind) {
    //         case ElementKind::POINTS:
    //             PointAttribute<T> a;
    //             return bindAttr<T>(name, PointAttribute<T>());
    //         case ElementKind::EDGES:
    //             return bindAttr<T>(name, EdgeAttribute<T>());
    //         case ElementKind::FACETS:
    //             return bindAttr<T>(name, FacetAttribute<T>());
    //         case ElementKind::CELL_FACETS:
    //             return bindAttr<T>(name, CellFacetAttribute<T>());
    //         case ElementKind::CORNERS:
    //             return bindAttr<T>(name, CornerAttribute<T>());
    //         case ElementKind::CELL_CORNERS:
    //             return bindAttr<T>(name, CellCornerAttribute<T>());
    //         case ElementKind::CELLS:
    //             return bindAttr<T>(name, CellAttribute<T>());
    //         default:
    //             throw std::runtime_error("Unknown element kind for binding attribute: " + elementKindToString(kind));
    //     }
    // }

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

    glm::vec3 getPosition() const {
        return position;
    }

    void setPosition(glm::vec3 p) {
        position = p;
    }

    bool getVisible() const {
        return visible;
    }

    void setVisible(bool v) {
        visible = v;
    }
    
    virtual bool getMeshVisible() const = 0;
    virtual void setMeshVisible(bool v) = 0;    
    virtual bool getPointVisible() const = 0;
    virtual void setPointVisible(bool v) = 0;
    virtual bool getEdgeVisible() const = 0;
    virtual void setEdgeVisible(bool v) = 0;

    std::shared_ptr<Model> getParent() const {
        return parent;
    }

    void setParent(std::shared_ptr<Model> parentModel) {
            parent = parentModel;
    }

    glm::vec3 getWorldPosition() const {
        if (parent) {
            return parent->getWorldPosition() + position;
        } else {
            return position;
        }
    }

    protected:
    std::string _name;
    std::string _path;

    glm::vec3 position{0, 0, 0};
    bool visible = true;

    // Pointer to parent model, if there is one
    std::shared_ptr<Model> parent;

    std::vector<Attribute> attrs;
    int selectedAttr = 0;


    bool isLightEnabled = true;
    bool isLightFollowView = false;

    bool isClipping = false;
    glm::vec3 clippingPlanePoint{0.f, 0.f, 0.f};
    glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
    bool invertClipping = false;

    float meshSize = 0.01f;
    float meshShrink = 0.f;
    
    RenderMode fragRenderMode = RenderMode::Color;
    ColorMode colorMode = ColorMode::COLOR;
    glm::vec3 color{0.8f, 0.f, 0.2f};
    
    int selectedColormap = 0;

    // std::unique_ptr<IRenderer> meshRenderer;
    // std::unique_ptr<IRenderer> pointSetRenderer;
    // std::unique_ptr<IRenderer> halfedgeRenderer;
};