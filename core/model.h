#pragma once

#include "element.h"
#include "attribute.h"
#include "renderer.h"
#include "color_mode.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"

#include "../include/glm/glm.hpp"
#include "../include/json.hpp"

#include <ultimaille/all.h>

#include <optional>

using namespace UM;
using json = nlohmann::json;

struct Model {
    struct AttrSelection {
        std::string attrName;
        ElementKind elementKind;
    };

    enum ModelType {
        POINTSET = 0,
        POLYLINE = 1,
        TRI = 2,
        QUAD = 3,
        TET = 4,
        HEX = 5,
        HYBRID = 6 
    };

    Model(std::map<std::string, std::shared_ptr<IRenderer>> renderers) :
    _name(""),
    _path(""),
    _renderers(std::move(renderers)) {}

    Model(std::map<std::string, std::shared_ptr<IRenderer>> renderers, std::string name) : 
    _name(name), 
    _path(""),
    _renderers(std::move(renderers)) {}

	template<typename T>
	T& as() {
        static_assert(std::is_base_of_v<Model, T>, "Model::as() can only be used with derived classes of Model");
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
        j["mesh_size"] = getMesh().getMeshSize();
        j["mesh_shrink"] = getMesh().getMeshShrink();
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

        getMesh().setMeshSize(model_state["mesh_size"].get<float>());
        getMesh().setMeshShrink(model_state["mesh_shrink"].get<float>());

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
	void init() {
        for (auto const &[k, r] : _renderers)
            r->init();
	}

    void push() {
        for (auto const &[k, r] : _renderers)
            r->push();

        if (colorMode == ColorMode::ATTRIBUTE) {
            updateAttr();
        }

        // Push highlight and filter attributes if they exist
        // TODO here update all layers in foreach
        updateHighlights();
        updateFilters();
    }

    void updateHighlights() {
        updateLayer(IRenderer::Layer::HIGHLIGHT);
    }

    // The same as TetModel
    void updateFilters() {
        updateLayer(IRenderer::Layer::FILTER);
    }

    virtual void updateLayer(IRenderer::Layer layer) = 0;

    void render() {
        if (!visible)
            return;
        
        glm::vec3 pos = getWorldPosition();

        for (auto const &[k, r] : _renderers)
            r->render(pos);
	}

    void clean() {
        for (auto const &[k, r] : _renderers)
            r->clean();
	}

    // Mesh info
    virtual int nverts() const = 0; 
    virtual int nfacets() const = 0; 
    virtual int ncells() const = 0; 
    virtual int ncorners() const = 0; 
    virtual std::tuple<glm::vec3, glm::vec3> bbox() = 0;

    glm::vec3 getCenter() {
        auto [bmin, bmax] = bbox();
        return (bmin + bmax) / 2.f;
    }


    // virtual void setFilter(int idx, bool filter) = 0;


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

    void setSelectedAttr(int idx) {

        // Check attrs size
        if (idx < 0 || idx >= attrs.size()) {
            throw std::runtime_error(
                "Selected attribute index out of bound: " + 
                std::to_string(idx) + ", model has " + 
                std::to_string(attrs.size()) + 
                " attributes."
            );
        }

        selectedAttr = idx;
        ElementKind kind = attrs[idx].kind;

        for (auto const &[k, r] : _renderers) {
            if (r->isRenderElement(kind)) {
                r->setAttribute(attrs[idx].ptr.get(), kind);
            }
        }
    }

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
    //             PointAttribute<T> pa;
    //             return bindAttr<T>(name, pa);
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

    int getSelectedColormap() const {
        return selectedColormap;
    }

    void setSelectedColormap(int idx) {
        for (auto const &[k, r] : _renderers)
            r->setSelectedColormap(idx);
            
        selectedColormap = idx;
    }

    // AttrSelection getHighlightAttr() const {
    //     return selectedAttrByLayer[IRenderer::Layer::HIGHLIGHT];
    // }

    void setHighlightAttr(std::string name, ElementKind kind) {
        setLayerAttr(name, kind, IRenderer::Layer::HIGHLIGHT);
    }

    // AttrSelection getFilterAttr() const {
    //     return selectedAttrByLayer[IRenderer::Layer::FILTER];
    // }

    void setFilterAttr(std::string name, ElementKind kind) {
        setLayerAttr(name, kind, IRenderer::Layer::FILTER);
    }

    // Choose which attribute to bind to layer
    void setLayerAttr(std::string name, ElementKind kind, IRenderer::Layer layer) {
        selectedAttrByLayer[layer].attrName = name;
        setLayer(kind, layer);
    }

    void setHighlight(ElementKind kind) {
        setLayer(kind, IRenderer::Layer::HIGHLIGHT);
    }

    void setFilter(ElementKind kind) {
        setLayer(kind, IRenderer::Layer::FILTER);
    }

    void unsetHighlight(ElementKind kind) {
        unsetLayer(kind, IRenderer::Layer::HIGHLIGHT);
    }

    void unsetHighlights() {
        // Unset all
        unsetHighlight(ElementKind::CELLS);
        unsetHighlight(ElementKind::FACETS);
        unsetHighlight(ElementKind::EDGES);
        unsetHighlight(ElementKind::POINTS);
    }

    void unsetFilter(ElementKind kind) {
        unsetLayer(kind, IRenderer::Layer::FILTER);
    }

    void unsetFilters() {
        // Unset all
        unsetFilter(ElementKind::POINTS);
        unsetFilter(ElementKind::CORNERS);
        unsetFilter(ElementKind::EDGES);
        unsetFilter(ElementKind::FACETS);
        unsetFilter(ElementKind::CELLS);
        unsetFilter(ElementKind::CELL_FACETS);
        unsetFilter(ElementKind::CELL_CORNERS);
    }

    void setLayer(ElementKind kind, IRenderer::Layer layer) {
        selectedAttrByLayer[layer].elementKind = kind;

        for (auto const &[k, r] : _renderers) {
            if (r->isRenderElement(kind)) {
                r->setLayerElement(kind, layer);
            }
        }

        updateLayer(layer);
    }

    void unsetLayer(ElementKind kind, IRenderer::Layer layer) {
        // Prepare a vector of zeros of the size of the element kind to unset
        std::vector<float> zeros;
        switch (kind) {
            case ElementKind::CELLS: {
                zeros.resize(ncells(), 0.f);
                break;
            }
            case ElementKind::CELL_FACETS:
            case ElementKind::FACETS: {
                zeros.resize(nfacets(), 0.f);
                break;
            }
            case ElementKind::EDGES: {
                // TODO implement nedges()
                break;
            } case ElementKind::CORNERS: {
                zeros.resize(ncorners(), 0.f);
                break;
            }
            case ElementKind::POINTS: {
                zeros.resize(nverts(), 0.f);
                break;
            }
        }

        // If renderer is rendering this kind of element, set requested layer data to zeros
        for (auto const &[k ,r] : _renderers) {
            if (r->isRenderElement(kind)) {
                r->setLayer(zeros, layer);
            }
        }
    }





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

    // Renderer functions
    void setTexture(unsigned int tex) {
        for (auto const &[k, r] : _renderers)
            r->setTexture(tex);
    }

    int getColorMode() const {
        return colorMode;
    }

    void setColorMode(ColorMode mode) {
        for (auto const &[k, r] : _renderers)
            r->setColorMode(mode);

        colorMode = mode;
    }

    bool getLight() const {
        return isLightEnabled;
    }

    void setLight(bool enabled) {
        for (auto const &[k, r] : _renderers)
            r->setLight(enabled);

        isLightEnabled = enabled;
    }

    bool getLightFollowView() const {
        return isLightFollowView;
    }

    void setLightFollowView(bool follow) {
        for (auto const &[k, r] : _renderers)
            r->setLightFollowView(follow);

        isLightFollowView = follow;
    }

    bool getClipping() const {
        return isClipping;
    }

    void setClipping(bool enabled) {
        for (auto const &[k, r] : _renderers)
            r->setClipping(enabled);

        isClipping = enabled;
    }

    glm::vec3 getClippingPlanePoint() const {
        return clippingPlanePoint;
    }

    void setClippingPlanePoint(glm::vec3 p) {
        for (auto const &[k, r] : _renderers)
            r->setClippingPlanePoint(p);

        clippingPlanePoint = p;
    }

    glm::vec3 getClippingPlaneNormal() const {
        return clippingPlaneNormal;
    }

    void setClippingPlaneNormal(glm::vec3 n) {
        for (auto const &[k, r] : _renderers)
            r->setClippingPlaneNormal(n);

        clippingPlaneNormal = n;
    }

    bool getInvertClipping() const {
        return invertClipping;
    }

    void setInvertClipping(bool invert) {
        for (auto const &[k, r] : _renderers)
            r->setInvertClipping(invert);

        invertClipping = invert;
    }

    void setMeshIndex(int index) {
        for (auto const &[k, r] : _renderers)
            r->setMeshIndex(index);
    }

    // Renderer getters
    PointSetRenderer& getPoints() {
        return *static_cast<PointSetRenderer*>(_renderers.at("point_renderer").get());
    }

    std::shared_ptr<HalfedgeRenderer> getEdges() {
        if (_renderers.contains("edge_renderer"))
            return  std::static_pointer_cast<HalfedgeRenderer>(_renderers.at("edge_renderer"));
            
        return nullptr;
    }

    IRenderer& getMesh() const {
        // Warning, I do that here because I have the garantee that _meshRenderer is always initialized
        // If _meshRenderer is uninitialized, this will throw a segfault
        // (I don't want to transfer ownership)
        // Maybe there is a better way to do that
        return *_renderers.at("mesh_renderer");
    }

    bool hasRenderer(std::string name) {
        return _renderers.contains(name);
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



    std::map<IRenderer::Layer, AttrSelection> selectedAttrByLayer{
        { IRenderer::Layer::HIGHLIGHT, {"_highlight", ElementKind::POINTS} },
        { IRenderer::Layer::FILTER, {"_filter", ElementKind::POINTS} },
    };

    bool isLightEnabled = true;
    bool isLightFollowView = false;

    bool isClipping = false;
    glm::vec3 clippingPlanePoint{0.f, 0.f, 0.f};
    glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
    bool invertClipping = false;
    
    ColorMode colorMode = ColorMode::COLOR;
    
    int selectedColormap = 0;

    // Renderers
    std::map<std::string, std::shared_ptr<IRenderer>> _renderers;

};