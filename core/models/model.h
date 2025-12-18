#pragma once

#include "../element.h"
#include "../layer.h"
#include "../attribute.h"
#include "../renderers/renderer.h"
#include "../color_mode.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"

#include "../../include/glm/glm.hpp"
#include "../../include/json.hpp"

#include <ultimaille/all.h>

#include <optional>

using namespace UM;
using json = nlohmann::json;

struct Model {
    struct AttrSelection {
        std::string attrName;
        ElementKind elementKind;
    };

    std::function<bool(Model& /*self*/, const std::string /*path*/)> loadCallback;

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


    void saveState(json &j) const {
        // Save current mesh state into a file
        auto now = std::chrono::system_clock::now();
        auto unix_timestamp = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
        auto filename = _name + "_" + unix_timestamp + ".geogram";
        saveAs(filename);

        j["name"] = _name;
        j["path"] = filename;
        j["position"] = { position.x, position.y, position.z };
        j["color_mode"] = colorMode;
        j["is_light_enabled"] = isLightEnabled;
        j["is_light_follow_view"] = isLightFollowView;
        j["is_clipping"] = isClipping;
        j["clipping_plane_point"] = { clippingPlanePoint.x, clippingPlanePoint.y, clippingPlanePoint.z };
        j["clipping_plane_normal"] = { clippingPlaneNormal.x, clippingPlaneNormal.y, clippingPlaneNormal.z };
        j["invert_clipping"] = invertClipping;
        j["selected_colormap"] = selectedColormap;
        j["selected_attr"] = selectedAttr;
        j["visible"] = visible;

        for (auto &[k, r] : _renderers) {
            r->saveState(j["renderers"][k]);
        }

        doSaveState(j);
    }

    void loadState(json &j) {
        
        _name = j["name"].get<std::string>();
        _path = j["path"].get<std::string>();

        position = glm::vec3(
            j["position"][0].get<float>(),
            j["position"][1].get<float>(),
            j["position"][2].get<float>()
        );
        
        setColorMode((ColorMode)j["color_mode"].get<int>());

        setLight(j["is_light_enabled"].get<bool>());
        setLightFollowView(j["is_light_follow_view"].get<bool>());
        setClipping(j["is_clipping"].get<bool>());

        setClippingPlanePoint(glm::vec3(
            j["clipping_plane_point"][0].get<float>(),
            j["clipping_plane_point"][1].get<float>(),
            j["clipping_plane_point"][2].get<float>()
        ));

        setClippingPlaneNormal(glm::vec3(
            j["clipping_plane_normal"][0].get<float>(),
            j["clipping_plane_normal"][1].get<float>(),
            j["clipping_plane_normal"][2].get<float>()
        ));

        setInvertClipping(j["invert_clipping"].get<bool>());

        int selectedAttr = j["selected_attr"].get<int>();
        if (selectedAttr >= 0)
            setSelectedAttr(selectedAttr);
            
        setSelectedColormap(j["selected_colormap"].get<int>());
        setVisible(j["visible"].get<bool>());

        // Load renderers state
        auto renderers = getRenderers();

        for (auto &[k, r] : renderers) {
            if (j["renderers"].contains(k))
                r->loadState(j["renderers"][k]);
        }

        doLoadState(j);
    }

    virtual void doLoadState(json &j) {};
    virtual void doSaveState(json &j) const {};

    std::string getName() const { return _name; }
    void setName(std::string name) { _name = name; }
    std::string getPath() const { return _path; }

    static constexpr const char* colorModeStrings[2] = {"Color", "Attribute"};
    constexpr std::array<std::string_view, 2> getColorModeStrings() {
        return {colorModeStrings[0], colorModeStrings[1]};
    }

    static constexpr const char* clippingModeStrings[2] = {"Cell", "Std"};
    constexpr std::array<std::string_view, 2> getClippingModeStrings() {
        return {clippingModeStrings[0], clippingModeStrings[1]};
    }

    // Lifecycle functions
	void init() {
        for (auto const &[k, r] : _renderers)
            r->init();
	}

    void push() {
        for (auto const &[k, r] : _renderers)
            r->push();

        std::cout << "update attr" << std::endl;

        if (colorMode == ColorMode::ATTRIBUTE) {
            updateAttr();
        }

        std::cout << "update hl / filter" << std::endl;

        // Push highlight and filter attributes if they exist
        // TODO here update all layers in foreach
        for (auto [k, isActivated] : activatedLayers) {
            if (!isActivated) 
                continue;

            auto [layer, kind] = k;
            updateLayer(layer, kind);
        }
    }

    void updateHighlights(ElementKind kind) {
        updateLayer(Layer::HIGHLIGHT, kind);
    }

    void updateFilters(ElementKind kind) {
        updateLayer(Layer::FILTER, kind);
    }


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
    virtual int nhalfedges() const = 0;
    virtual std::tuple<glm::vec3, glm::vec3> bbox() = 0;

    glm::vec3 getCenter() {
        auto [bmin, bmax] = bbox();
        return (bmin + bmax) / 2.f;
    }

    double getRadius() {
        auto [bmin, bmax] = bbox();
        return glm::length(bmax - bmin) / 2.f;
    }

    void addAttr(ElementKind kind, NamedContainer &container) {
        
        // Get the type of the container
        ElementType type = ElementType::DOUBLE_ELT; // Default type
        if (auto a = dynamic_cast<AttributeContainer<double>*>(container.ptr.get())) {
            type = ElementType::DOUBLE_ELT;
        } else if (auto a = dynamic_cast<AttributeContainer<int>*>(container.ptr.get())) {
            type = ElementType::INT_ELT;
        } else if (auto a = dynamic_cast<AttributeContainer<bool>*>(container.ptr.get())) {
            type = ElementType::BOOL_ELT;
        } else if (auto a = dynamic_cast<AttributeContainer<vec2>*>(container.ptr.get())) {
            type = ElementType::VEC2_ELT;
            attrs.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, 0);
            attrs.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, 1);
        } else {
            throw std::runtime_error("Unknown attribute type for container: " + container.name);
        }

        attrs.emplace_back(container.name, kind, type, container.ptr, -1);
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

        // Under 0, no selection
        if (idx < 0) {
            return;
        }

        if (idx >= attrs.size()) {
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
            r->setAttrElement(kind);
            if (r->isRenderElement(kind)) {
                r->setAttribute(attrs[idx]);
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
            return ElementType::DOUBLE_ELT;
        } else if constexpr (std::is_same_v<T, int>) {
            return ElementType::INT_ELT;
        } else if constexpr (std::is_same_v<T, bool>) {
            return ElementType::BOOL_ELT;
        } else if constexpr (std::is_same_v<T, vec2>) {
            return ElementType::VEC2_ELT;
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
            case AttributeBase::POINTS: return ElementKind::POINTS_ELT;
            case AttributeBase::EDGES: return ElementKind::EDGES_ELT;
            case AttributeBase::FACETS: return ElementKind::FACETS_ELT;
            case AttributeBase::CORNERS: return ElementKind::CORNERS_ELT;
            case AttributeBase::CELLS: return ElementKind::CELLS_ELT;
            case AttributeBase::CELLFACETS: return ElementKind::CELL_FACETS_ELT;
            case AttributeBase::CELLCORNERS: return ElementKind::CELL_CORNERS_ELT;
            default: throw std::runtime_error("Unknown attribute kind for binding: " + std::to_string(kind));
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

    int getSelectedColormap() const {
        return selectedColormap;
    }

    void setSelectedColormap(int idx) {
        for (auto const &[k, r] : _renderers)
            r->setSelectedColormap(idx);
            
        selectedColormap = idx;
    }

    // Choose which attribute to bind to layer / kind
    void setLayerAttr(std::string name, Layer layer, ElementKind kind) {
        attrNameByLayerAndKind[{layer, kind}] = name;
    }

    std::string getLayerAttr(Layer layer, ElementKind kind) {
        std::tuple<Layer, ElementKind> k = {layer, kind};
        if (attrNameByLayerAndKind.contains(k))
            return attrNameByLayerAndKind[k];
        
        return defaultAttrName(layer);
    }

    void setHighlightAttr(std::string name, ElementKind kind) {
        setLayerAttr(name, Layer::HIGHLIGHT, kind);
    }

    void setFilterAttr(std::string name, ElementKind kind) {
        setLayerAttr(name, Layer::FILTER, kind);
    }

    void setHighlight(ElementKind kind) {
        setLayer(kind, Layer::HIGHLIGHT);
    }

    void setFilter(ElementKind kind) {
        setLayer(kind, Layer::FILTER);
    }

    void unsetHighlight(ElementKind kind) {
        unsetLayer(kind, Layer::HIGHLIGHT);
    }

    void unsetHighlights() {
        // Unset all
        unsetHighlight(ElementKind::POINTS_ELT);
        unsetHighlight(ElementKind::CORNERS_ELT);
        unsetHighlight(ElementKind::EDGES_ELT);
        unsetHighlight(ElementKind::FACETS_ELT);
        unsetHighlight(ElementKind::CELLS_ELT);
        unsetHighlight(ElementKind::CELL_FACETS_ELT);
        unsetHighlight(ElementKind::CELL_CORNERS_ELT);
    }

    void unsetFilter(ElementKind kind) {
        unsetLayer(kind, Layer::FILTER);
    }

    void unsetFilters() {
        // Unset all
        unsetFilter(ElementKind::POINTS_ELT);
        unsetFilter(ElementKind::CORNERS_ELT);
        unsetFilter(ElementKind::EDGES_ELT);
        unsetFilter(ElementKind::FACETS_ELT);
        unsetFilter(ElementKind::CELLS_ELT);
        unsetFilter(ElementKind::CELL_FACETS_ELT);
        unsetFilter(ElementKind::CELL_CORNERS_ELT);
    }

    void setLayer(ElementKind kind, Layer layer) {

        for (auto const &[k, r] : _renderers) {
            if (r->isRenderElement(kind)) {
                r->setLayerElement(kind, layer);
            }
        }

        updateLayer(layer, kind);

        activatedLayers[{layer, kind}] = true;
    }

    void unsetLayer(ElementKind kind, Layer layer) {
        // Prepare a vector of zeros of the size of the element kind to unset
        std::vector<float> zeros;
        switch (kind) {
            case ElementKind::CELLS_ELT: {
                zeros.resize(ncells(), 0.f);
                break;
            }
            case ElementKind::CELL_FACETS_ELT:
            case ElementKind::FACETS_ELT: {
                zeros.resize(nfacets(), 0.f);
                break;
            }
            case ElementKind::EDGES_ELT: {
                zeros.resize(nhalfedges(), 0.f);
                break;
            } 
            case ElementKind::CORNERS_ELT: 
            case ElementKind::CELL_CORNERS_ELT: {
                zeros.resize(ncorners(), 0.f);
                break;
            }
            case ElementKind::POINTS_ELT: {
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

        activatedLayers[{layer, kind}] = false;
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

    IRenderer::ClippingMode getClippingMode() const {
        return clippingMode;
    }

    void setClippingMode(IRenderer::ClippingMode mode) {
        for (auto const &[k, r] : _renderers)
            r->setClippingMode(mode);

        clippingMode = mode;
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

    void setupClipping() {
        setClippingPlanePoint(getCenter());
        setClippingPlaneNormal({1, 0, 0});
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

    std::map<std::string, std::shared_ptr<IRenderer>> getRenderers() const {
        return _renderers;
    }

    // TODO maybe remove that later, using screen RBO to get edge ?
    virtual long pick_edge(glm::vec3 p0, int c) = 0;

    protected:
    std::string _name;
    std::string _path;

    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    bool visible = true;

    // Pointer to parent model, if there is one
    std::shared_ptr<Model> parent;

    std::vector<Attribute> attrs;
    int selectedAttr = 0;

    bool isLightEnabled = true;
    bool isLightFollowView = false;

    IRenderer::ClippingMode clippingMode = IRenderer::ClippingMode::STD;
    bool isClipping = false;
    glm::vec3 clippingPlanePoint{0.f, 0.f, 0.f};
    glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
    bool invertClipping = false;
    
    ColorMode colorMode = ColorMode::COLOR;
    
    int selectedColormap = 0;

    // Renderers
    std::map<std::string, std::shared_ptr<IRenderer>> _renderers;

    virtual void updateLayer(Layer layer, ElementKind kind) = 0;


    private:
    
    std::map<std::tuple<Layer, ElementKind>, std::string> attrNameByLayerAndKind;
    std::map<std::tuple<Layer, ElementKind>, bool> activatedLayers;
};