#pragma once

#include <json.hpp>
#include <ultimaille/all.h>
#include <string>
#include "core/model.h"
#include "hex_renderer.h"
#include "point_set_renderer.h"

using namespace UM;
using json = nlohmann::json;

struct HexModel final : public Model {

	// Mesh + Renderer

	HexModel() : 
        _name(""), 
        _path(""), 
        _hex(), 
        _hexRenderer(_hex), 
        _pointSetRenderer(_hex.points) {}

	HexModel(std::string name) : 
		_name(name), 
		_path(""), 
		_hex(),
		_hexRenderer(_hex),
        _pointSetRenderer(_hex.points) {}


	void load(const std::string path);
	void save() const override;
	void save_as(const std::string path) const override;

    std::string save_state() override {
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
        
        setColorMode((Model::ColorMode)model_state["color_mode"].get<int>());

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

    std::string getName() const override { return _name; }
    void setName(std::string name) override { _name = name; }
    std::string getPath() const override { return _path; }

	void init() override {
		_hexRenderer.init();
		_pointSetRenderer.init();
	}

    void push() override;

    void render() override {
        if (!visible)
            return;
        
        glm::vec3 pos = getWorldPosition();

        _hexRenderer.render(pos);
        _pointSetRenderer.render(pos);
	}
    
    void clean() override {
		_hexRenderer.clean();
        _pointSetRenderer.clean();
	}

	Hexahedra& getHexahedra() override { return _hex; }
	// HexRenderer& getRenderer() { return _hexRenderer; }

    void setTexture(unsigned int tex) override {
        _hexRenderer.setTexture(tex);
        _pointSetRenderer.setTexture(tex);
    }

	// Just call underlying renderer methods
    int getColorMode() const override {
        return colorMode;
    }

    void setColorMode(Model::ColorMode mode) override {
        _hexRenderer.setColorMode(mode);
        _pointSetRenderer.setColorMode(mode);
        colorMode = mode;
    }

    glm::vec3 getColor() const override {
        return color;
    }

    void setColor(glm::vec3 c) override {
        _hexRenderer.setColor(c);
        color = c;
    }

    bool getLight() const override {
        return isLightEnabled;
    }

    void setLight(bool enabled) override {
        _hexRenderer.setLight(enabled);
        isLightEnabled = enabled;
    }

    bool getLightFollowView() const override {
        return isLightFollowView;
    }

    void setLightFollowView(bool follow) override {
		_hexRenderer.setLightFollowView(follow);
        isLightFollowView = follow;
    }

    bool getClipping() const override {
        return isClipping;
    }

    void setClipping(bool enabled) override {
        _hexRenderer.setClipping(enabled);
        _pointSetRenderer.setClipping(enabled);
        isClipping = enabled;
    }

    glm::vec3 getClippingPlanePoint() const override {
        return clippingPlanePoint;
    }

    void setClippingPlanePoint(glm::vec3 p) override {
        _hexRenderer.setClippingPlanePoint(p);
        _pointSetRenderer.setClippingPlanePoint(p);
        clippingPlanePoint = p;
    }

    glm::vec3 getClippingPlaneNormal() const override {
        return clippingPlaneNormal;
    }

    void setClippingPlaneNormal(glm::vec3 n) override {
        _hexRenderer.setClippingPlaneNormal(n);
        _pointSetRenderer.setClippingPlaneNormal(n);
        clippingPlaneNormal = n;
    }

    bool getInvertClipping() const override {
        return invertClipping;
    }

    void setInvertClipping(bool invert) override {
        _hexRenderer.setInvertClipping(invert);
        _pointSetRenderer.setInvertClipping(invert);
        invertClipping = invert;
    }

    float getMeshSize() const override {
        return meshSize;
    }

    void setMeshSize(float val) override {
        _hexRenderer.setMeshSize(val);
        meshSize = val;
    }

    float getMeshShrink() const override {
        return meshShrink;
    }

    void setMeshShrink(float val) override {
        _hexRenderer.setMeshShrink(val);
        meshShrink = val;
    }

    int getFragRenderMode() const override {
        return fragRenderMode;
    }

    void setFragRenderMode(Model::RenderMode mode) override {
        _hexRenderer.setFragRenderMode(mode);
        fragRenderMode = mode;
    }

    int getSelectedColormap() const override {
        return selectedColormap;
    }

    void setSelectedColormap(int idx) override {
        _hexRenderer.setSelectedColormap(idx);
        selectedColormap = idx;
    }

    glm::vec3 getPointColor() const override {
        return _pointSetRenderer.getPointColor();
    }

    void setPointColor(glm::vec3 color) override {
        _pointSetRenderer.setPointColor(color);
    }

    float getPointSize() const override {
        return _pointSetRenderer.getPointSize();
    }

    void setPointSize(float size) override {
        _pointSetRenderer.setPointSize(size);
    }

    glm::vec3 getWorldPosition() const override {
        if (parent) {
            return parent->getWorldPosition() + position;
        } else {
            return position;
        }
    }

    glm::vec3 getPosition() const override {
        return position;
    }

    void setPosition(glm::vec3 p) override {
        position = p;
    }

    bool getVisible() const override {
        return visible;
    }

    void setVisible(bool v) override {
        visible = v;
    }

    bool getPointVisible() const override {
        return _pointSetRenderer.getVisible();
    }

    void setPointVisible(bool v) override {
        _pointSetRenderer.setVisible(v);
    }

    void setHighlight(int idx, float highlight) override {
        _hexRenderer.setHighlight(idx, highlight);
    }

    void setHighlight(std::vector<float> highlights) override {
        _hexRenderer.setHighlight(highlights);
    }

    void setFacetHighlight(int idx, float highlight) override {
        _hexRenderer.setFacetHighlight(idx, highlight);
    }

    void setFacetHighlight(std::vector<float> highlights) override {
        _hexRenderer.setFacetHighlight(highlights);
    }

    // TODO filter anything else than cell !
    void setFilter(int idx, bool filter) override {
        _hexRenderer.setFilter(idx, filter);
        _hexRenderer.setFacetHighlight(rand() % _hex.nfacets(), 1.0f);

        // TODO it works but... not very efficient !
        Volume::Cell c(_hex, idx);
        for (int lc = 0; lc < 8; ++lc) {
            auto corner = c.corner(lc);
            auto v = corner.vertex();
            // Retrieve all cells attached to this point to see whether filtered
            bool allFiltered = true;
            // #pragma omp parallel for
            for (int i = 0; i < _hex.cells.size(); ++i) {
                if (_hex.cells[i] != v)
                    continue;
                
                int ci = i / 8;
                if (_hexRenderer.getFilterPtr()[ci] <= 0) {
                    allFiltered = false;
                    break;
                }
            }

            _pointSetRenderer.setFilter(v, allFiltered);
            // _pointSetRenderer.setHighlight(v, 0.1f);
        }

        
    }


    std::vector<Attribute> getAttrs() const override {
        return attrs;
    }

    Attribute getAttr(int idx) const override {
        return attrs[idx];
    }

    void clearAttrs() override {
        attrs.clear();
    }

    int getSelectedAttr() const override {
        return selectedAttr;
    }

    void setSelectedAttr(int idx) override;

    void updateAttr() {
        setSelectedAttr(selectedAttr);
    }

    std::shared_ptr<Model> getParent() const override {
        return parent;
    }

    void setParent(std::shared_ptr<Model> parentModel) override {
            parent = parentModel;
    }

    private:
    std::string _name;
    std::string _path;

    glm::vec3 position{0, 0, 0};

    bool visible = true;

    bool isLightEnabled = true;
    bool isLightFollowView = false;

    bool isClipping = false;
    glm::vec3 clippingPlanePoint{0.f, 0.f, 0.f};
    glm::vec3 clippingPlaneNormal{0.f, 0.f, 1.f};
    bool invertClipping = false;
    float meshSize = 0.01f;
    float meshShrink = 0.f;
    Model::RenderMode fragRenderMode = Model::RenderMode::Color;
    Model::ColorMode colorMode = Model::ColorMode::COLOR;
    glm::vec3 color{0.8f, 0.f, 0.2f};
    
    int selectedAttr = 0;
    int selectedColormap = 0;

    // Mesh
    Hexahedra _hex;

    // Renderers
    HexRenderer _hexRenderer;
    PointSetRenderer _pointSetRenderer;

    // std::vector<std::tuple<std::string, Element, std::shared_ptr<ContainerBase>>> attrs;
    



    std::vector<Attribute> attrs;

    // Pointer to parent model, if there is one
    std::shared_ptr<Model> parent;

    void addAttr(Element element, NamedContainer &container) {

        // Get the type of the container
        ElementType type = ElementType::FLOAT; // Default type
        if (auto a = dynamic_cast<AttributeContainer<double>*>(container.ptr.get())) {
            type = ElementType::DOUBLE;
        } else if (auto a = dynamic_cast<AttributeContainer<int>*>(container.ptr.get())) {
            type = ElementType::INT;
        } else if (auto a = dynamic_cast<AttributeContainer<bool>*>(container.ptr.get())) {
            type = ElementType::BOOL;
        } else if (auto a = dynamic_cast<AttributeContainer<float>*>(container.ptr.get())) {
            type = ElementType::FLOAT;
        } else {
            throw std::runtime_error("Unknown attribute type for container: " + container.name);
        }

        attrs.emplace_back(container.name, element, type, container.ptr);
    }

};