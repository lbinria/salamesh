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

    Model(std::unique_ptr<IRenderer> meshRenderer, PointSetRenderer pointSetRenderer, std::shared_ptr<HalfedgeRenderer> halfedgeRenderer) :
    _name(""),
    _path(""),
    _meshRenderer(std::move(meshRenderer)),
    _pointSetRenderer(std::move(pointSetRenderer)),
    _halfedgeRenderer(std::move(halfedgeRenderer)) {}

    Model(std::unique_ptr<IRenderer> meshRenderer, PointSetRenderer pointSetRenderer, std::shared_ptr<HalfedgeRenderer> halfedgeRenderer, std::string name) : 
    _name(name), 
    _path(""),
    _meshRenderer(std::move(meshRenderer)),
    _pointSetRenderer(std::move(pointSetRenderer)),
    _halfedgeRenderer(std::move(halfedgeRenderer)) {}

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
		_meshRenderer->init();
		_pointSetRenderer.init();
        if (_halfedgeRenderer != nullptr)
            _halfedgeRenderer->init();
	}

    void push() {
        _meshRenderer->push();
        _pointSetRenderer.push();
        if (_halfedgeRenderer != nullptr)
            _halfedgeRenderer->push();

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

        _meshRenderer->render(pos);
        _pointSetRenderer.render(pos);
        if (_halfedgeRenderer != nullptr)
            _halfedgeRenderer->render(pos);
	}

    void clean() {
		_meshRenderer->clean();
        _pointSetRenderer.clean();
        if (_halfedgeRenderer != nullptr)
            _halfedgeRenderer->clean();
	}

    // Mesh info
    virtual int nverts() const = 0; 
    virtual int nfacets() const = 0; 
    virtual int ncells() const = 0; 



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
        int kind = attrs[idx].kind;
        // TODO see condition here not very smart maybe abstract renderers ?
        if (kind == ElementKind::POINTS) {
            _pointSetRenderer.setAttribute(attrs[idx].ptr.get(), -1);
        } else 
            _meshRenderer->setAttribute(attrs[idx].ptr.get(), kind);
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

    // virtual void removeAttr(const std::string& name, ElementKind element) = 0;

    // Model attributes shader uniforms
    // virtual int getSelectedColormap() const = 0;
    // virtual void setSelectedColormap(int idx) = 0;

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
        _meshRenderer->setSelectedColormap(idx);
        _pointSetRenderer.setSelectedColormap(idx);
        if (_halfedgeRenderer != nullptr)
            _halfedgeRenderer->setSelectedColormap(idx);
            
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
        unsetFilter(ElementKind::CELLS);
        unsetFilter(ElementKind::FACETS);
        unsetFilter(ElementKind::EDGES);
        unsetFilter(ElementKind::POINTS);
    }

    void setLayer(ElementKind kind, IRenderer::Layer layer) {
        selectedAttrByLayer[layer].elementKind = kind;

        switch (kind) {
            case ElementKind::CELLS:
            case ElementKind::CELL_FACETS:
            case ElementKind::FACETS:
                _meshRenderer->setLayerElement(kind, layer);
            break;
            case ElementKind::EDGES:
            case ElementKind::CORNERS:
                if (_halfedgeRenderer)
                    _halfedgeRenderer->setLayerElement(kind, layer);
            break;
            case ElementKind::POINTS:
                _pointSetRenderer.setLayerElement(kind, layer);
            break;
            default:
            
            break;
        }

        updateLayer(layer);
    }

    void unsetLayer(ElementKind kind, IRenderer::Layer layer) {
        switch (kind) {
            case ElementKind::CELLS: {
                std::vector<float> zeros(ncells(), 0.f);
                _meshRenderer->setLayer(zeros, layer);
                break;
            }
            case ElementKind::CELL_FACETS:
            case ElementKind::FACETS: {
                std::vector<float> zeros(nfacets(), 0.f);
                _meshRenderer->setLayer(zeros, layer);
                break;
            }
            case ElementKind::EDGES:
            case ElementKind::CORNERS: {
                // TODO implement nhalfedges()
                // std::vector<float> zeros(ned(), 0.f);
                // _meshRenderer->setLayer(zeros, layer);
                break;
            }
            case ElementKind::POINTS: {
                std::vector<float> zeros(nverts(), 0.f);
                _pointSetRenderer.setLayer(zeros, layer);
                break;
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
        _meshRenderer->setTexture(tex);
        _pointSetRenderer.setTexture(tex);
        if (_halfedgeRenderer != nullptr)
            _halfedgeRenderer->setTexture(tex);
    }

    int getColorMode() const {
        return colorMode;
    }

    void setColorMode(ColorMode mode) {
        _meshRenderer->setColorMode(mode);
        _pointSetRenderer.setColorMode(mode);
        if (_halfedgeRenderer != nullptr)
            _halfedgeRenderer->setColorMode(mode);
        colorMode = mode;
    }

    bool getLight() const {
        return isLightEnabled;
    }

    void setLight(bool enabled) {
        _meshRenderer->setLight(enabled);
        // _pointSetRenderer.setLight(enabled); 
        // _halfedgeRenderer->setLight(enabled); 
        isLightEnabled = enabled;
    }

    bool getLightFollowView() const {
        return isLightFollowView;
    }

    void setLightFollowView(bool follow) {
		_meshRenderer->setLightFollowView(follow);
        isLightFollowView = follow;
    }

    bool getClipping() const {
        return isClipping;
    }

    void setClipping(bool enabled) {
        _meshRenderer->setClipping(enabled);
        _pointSetRenderer.setClipping(enabled);
        isClipping = enabled;
    }

    glm::vec3 getClippingPlanePoint() const {
        return clippingPlanePoint;
    }

    void setClippingPlanePoint(glm::vec3 p) {
        _meshRenderer->setClippingPlanePoint(p);
        _pointSetRenderer.setClippingPlanePoint(p);
        clippingPlanePoint = p;
    }

    glm::vec3 getClippingPlaneNormal() const {
        return clippingPlaneNormal;
    }

    void setClippingPlaneNormal(glm::vec3 n) {
        _meshRenderer->setClippingPlaneNormal(n);
        _pointSetRenderer.setClippingPlaneNormal(n);
        clippingPlaneNormal = n;
    }

    bool getInvertClipping() const {
        return invertClipping;
    }

    void setInvertClipping(bool invert) {
        _meshRenderer->setInvertClipping(invert);
        _pointSetRenderer.setInvertClipping(invert);
        invertClipping = invert;
    }

    void setMeshIndex(int index) {
        _meshRenderer->setMeshIndex(index);
    }

    // Renderer getters
    PointSetRenderer& getPoints() {
        return _pointSetRenderer;
    }

    std::shared_ptr<HalfedgeRenderer> getEdges() {
        return _halfedgeRenderer;
    }

    IRenderer& getMesh() const {
        // Warning, I do that here because I have the garantee that _meshRenderer is always initialized
        // If _meshRenderer is uninitialized, this will throw a segfault
        // (I don't want to transfer ownership)
        // Maybe there is a better way to do that
        return *_meshRenderer;
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
    std::unique_ptr<IRenderer> _meshRenderer;
    PointSetRenderer _pointSetRenderer;
    std::shared_ptr<HalfedgeRenderer> _halfedgeRenderer;

    std::map<std::string, std::shared_ptr<IRenderer>> _renderers;

};