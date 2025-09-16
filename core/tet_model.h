#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "tet_renderer.h"

using namespace UM;
using json = nlohmann::json;

struct TetModel final : public Model {

	// Mesh + Renderer

    using Model::Model;

	TetModel() : 
        _m(), 
        _tetRenderer(_m), 
        _pointSetRenderer(_m.points)
        /*_halfedgeRenderer(_m)*/ {
        }

    ModelType getModelType() const override {
        return ModelType::TET;
    }


	bool load(const std::string path) override;
	void save() const override;
	void saveAs(const std::string path) const override;

    std::string save_state() const override;
    void load_state(json model_state);


	void init() override {
		_tetRenderer.init();
		_pointSetRenderer.init();
        // _halfedgeRenderer.init();
	}

    void push() override;

    void render() override {
        if (!visible)
            return;
        
        glm::vec3 pos = getWorldPosition();

        _tetRenderer.render(pos);
        _pointSetRenderer.render(pos);
        // _halfedgeRenderer.render(pos);
	}
    
    void clean() override {
		_tetRenderer.clean();
        _pointSetRenderer.clean();
        // _halfedgeRenderer.clean();
	}

	Tetrahedra& getTetrahedra() { return _m; }
	VolumeAttributes& getVolumeAttributes() { return _volumeAttributes; }

    int nverts() const override {
        return _m.nverts();
    }

    int nfacets() const override {
        return _m.nfacets();
    } 

    int ncells() const override {
        return _m.ncells();
    } 


    void setTexture(unsigned int tex) override {
        _tetRenderer.setTexture(tex);
        _pointSetRenderer.setTexture(tex);
        // _halfedgeRenderer.setTexture(tex);
    }

    int getColorMode() const override {
        return colorMode;
    }

    void setColorMode(Model::ColorMode mode) override {
        _tetRenderer.shader.setColorMode(mode);
        _pointSetRenderer.setColorMode(mode);
        // _halfedgeRenderer.setColorMode(mode);
        colorMode = mode;
    }

    glm::vec3 getColor() const override {
        return color;
    }

    void setColor(glm::vec3 c) override {
        _tetRenderer.shader.setColor(c);
        color = c;
    }

    bool getLight() const override {
        return isLightEnabled;
    }

    void setLight(bool enabled) override {
        _tetRenderer.shader.setLight(enabled);
        isLightEnabled = enabled;
    }

    bool getLightFollowView() const override {
        return isLightFollowView;
    }

    void setLightFollowView(bool follow) override {
		_tetRenderer.shader.setLightFollowView(follow);
        isLightFollowView = follow;
    }

    bool getClipping() const override {
        return isClipping;
    }

    void setClipping(bool enabled) override {
        _tetRenderer.shader.setClipping(enabled);
        _pointSetRenderer.setClipping(enabled);
        isClipping = enabled;
    }

    glm::vec3 getClippingPlanePoint() const override {
        return clippingPlanePoint;
    }

    void setClippingPlanePoint(glm::vec3 p) override {
        _tetRenderer.shader.setClippingPlanePoint(p);
        _pointSetRenderer.setClippingPlanePoint(p);
        clippingPlanePoint = p;
    }

    glm::vec3 getClippingPlaneNormal() const override {
        return clippingPlaneNormal;
    }

    void setClippingPlaneNormal(glm::vec3 n) override {
        _tetRenderer.shader.setClippingPlaneNormal(n);
        _pointSetRenderer.setClippingPlaneNormal(n);
        clippingPlaneNormal = n;
    }

    bool getInvertClipping() const override {
        return invertClipping;
    }

    void setInvertClipping(bool invert) override {
        _tetRenderer.shader.setInvertClipping(invert);
        _pointSetRenderer.setInvertClipping(invert);
        invertClipping = invert;
    }

    float getMeshSize() const override {
        return meshSize;
    }

    void setMeshSize(float val) override {
        _tetRenderer.shader.setMeshSize(val);
        meshSize = val;
    }

    float getMeshShrink() const override {
        return meshShrink;
    }

    void setMeshShrink(float val) override {
        _tetRenderer.shader.setMeshShrink(val);
        meshShrink = val;
    }

    bool getMeshVisible() const override {
        return _tetRenderer.getVisible();
    }

    void setMeshVisible(bool visible) override {
        return _tetRenderer.setVisible(visible);
    }

    int getFragRenderMode() const override {
        return fragRenderMode;
    }

    void setFragRenderMode(Model::RenderMode mode) override {
        _tetRenderer.shader.setFragRenderMode(mode);
        fragRenderMode = mode;
    }

    int getSelectedColormap() const override {
        return selectedColormap;
    }

    void setSelectedColormap(int idx) override {
        _tetRenderer.shader.setSelectedColormap(idx);
        selectedColormap = idx;
    }

    glm::vec3 getPointColor() const override {
        return _pointSetRenderer.getColor();
    }

    void setPointColor(glm::vec3 color) override {
        _pointSetRenderer.setColor(color);
    }

    float getPointSize() const override {
        return _pointSetRenderer.getPointSize();
    }

    void setPointSize(float size) override {
        _pointSetRenderer.setPointSize(size);
    }

    float getEdgeSize() const override {
        // return _halfedgeRenderer.getEdgeSize();
		return 0;
    }

    void setEdgeSize(float size) override {
        // _halfedgeRenderer.setEdgeSize(size);
    }

    glm::vec3 getEdgeInsideColor() const override {
		return glm::vec3(0);
        // return _halfedgeRenderer.getEdgeInsideColor();
    }

    void setEdgeInsideColor(glm::vec3 color) override {
        // _halfedgeRenderer.setEdgeInsideColor(color);
    }

    glm::vec3 getEdgeOutsideColor() const override {
		return glm::vec3(0);
        // return _halfedgeRenderer.getEdgeOutsideColor();
    }

    void setEdgeOutsideColor(glm::vec3 color) override {
        // _halfedgeRenderer.setEdgeOutsideColor(color);
    }

    void setMeshIndex(int index) override {
        _tetRenderer.shader.setMeshIndex(index);
    }

    bool getPointVisible() const override {
        return _pointSetRenderer.getVisible();
    }

    void setPointVisible(bool v) override {
        _pointSetRenderer.setVisible(v);
    }

    bool getEdgeVisible() const override {
		return false;
        // return _halfedgeRenderer.getVisible();
    }

    void setEdgeVisible(bool v) override {
        // _halfedgeRenderer.setVisible(v);
    }

    void setHighlight(int idx, float highlight) override {
        _tetRenderer.setHighlight(idx, highlight);
    }

    void setHighlight(std::vector<float> highlights) override {
        _tetRenderer.setHighlight(highlights);
    }

    void setFacetHighlight(int idx, float highlight) {
        _tetRenderer.setFacetHighlight(idx, highlight);
    }

    void setFacetHighlight(std::vector<float> highlights) {
        _tetRenderer.setFacetHighlight(highlights);
    }

    void setPointHighlight(int idx, float highlight) override {
        _pointSetRenderer.setHighlight(idx, highlight);
    }

    void setPointHighlight(std::vector<float> highlights) override {
        _pointSetRenderer.setHighlight(highlights);
    }

    // TODO filter anything else than cell !
    void setFilter(int idx, bool filter) override {
        _tetRenderer.setFilter(idx, filter);

        // TODO it works but... not very efficient !
        Volume::Cell c(_m, idx);
        for (int lc = 0; lc < 8; ++lc) {
            auto corner = c.corner(lc);
            auto v = corner.vertex();
            // Retrieve all cells attached to this point to see whether filtered
            bool allFiltered = true;
            // #pragma omp parallel for
            for (int i = 0; i < _m.cells.size(); ++i) {
                if (_m.cells[i] != v)
                    continue;
                
                int ci = i / 8;
                if (_tetRenderer.getFilterPtr()[ci] <= 0) {
                    allFiltered = false;
                    break;
                }
            }

            _pointSetRenderer.setFilter(v, allFiltered);
            // _pointSetRenderer.setHighlight(v, 0.1f);
        }

        
    }


    void setSelectedAttr(int idx) override;
    void setSelectedAttr(std::string name, ElementKind kind) override;

    private:

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
    
    int selectedColormap = 0;

    // Mesh
    Tetrahedra _m;
    VolumeAttributes _volumeAttributes;

    // Renderers
    PointSetRenderer _pointSetRenderer;
    // HalfedgeRenderer _halfedgeRenderer;
    TetRenderer _tetRenderer;

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
};