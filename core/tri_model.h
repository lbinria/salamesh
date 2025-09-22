#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "tri_renderer.h"
#include "color_mode.h"
#include "render_mode.h"

using namespace UM;
using json = nlohmann::json;

struct TriModel final : public Model {

	// Mesh + Renderer
    using Model::Model;

	TriModel() : 
        _tri(), 
        Model::Model(std::make_unique<TriRenderer>(_tri), PointSetRenderer(_tri.points), std::nullopt),
        _triRenderer(_tri)
        // _pointSetRenderer(_tri.points)
        /* _halfedgeRenderer(_tri)*/ {}


    ModelType getModelType() const override {
        return ModelType::TRI;
    }


	bool load(const std::string path) override;
	// void save() const override;
	void saveAs(const std::string path) const override;


	// std::string save_state() const override;
	// void load_state(json model_state);

	void init() override {
		_triRenderer.init();
		_pointSetRenderer.init();
        // _halfedgeRenderer.init();
	}

    void push() override;

    void render() override {
        if (!visible)
            return;
        
        glm::vec3 pos = getWorldPosition();

        _triRenderer.render(pos);
        _pointSetRenderer.render(pos);
        // _halfedgeRenderer.render(pos);
	}
    
    void clean() override {
		_triRenderer.clean();
        _pointSetRenderer.clean();
        // _halfedgeRenderer.clean();
	}


	Triangles& getTriangles() { return _tri; }
	SurfaceAttributes& getSurfaceAttributes() { return _surfaceAttributes; }

    int nverts() const override {
        return _tri.nverts();
    }

    int nfacets() const override {
        return _tri.nfacets();
    } 

    int ncells() const override {
        return 0;
    } 

    void setTexture(unsigned int tex) override {
        _triRenderer.setTexture(tex);
        _pointSetRenderer.setTexture(tex);
        // _halfedgeRenderer.setTexture(tex);
    }

	// Just call underlying renderer methods
    int getColorMode() const override {
        return colorMode;
    }

    void setColorMode(ColorMode mode) override {
        _triRenderer.setColorMode(mode);
        _pointSetRenderer.setColorMode(mode);
        // _halfedgeRenderer.setColorMode(mode);
        colorMode = mode;
    }

    glm::vec3 getColor() const override {
        return color;
    }

    void setColor(glm::vec3 c) override {
        _triRenderer.setColor(c);
        color = c;
    }

    bool getLight() const override {
        return isLightEnabled;
    }

    void setLight(bool enabled) override {
        _triRenderer.setLight(enabled);
        isLightEnabled = enabled;
    }

    bool getLightFollowView() const override {
        return isLightFollowView;
    }

    void setLightFollowView(bool follow) override {
		_triRenderer.setLightFollowView(follow);
        isLightFollowView = follow;
    }

    bool getClipping() const override {
        return isClipping;
    }

    void setClipping(bool enabled) override {
        _triRenderer.setClipping(enabled);
        _pointSetRenderer.setClipping(enabled);
        isClipping = enabled;
    }

    glm::vec3 getClippingPlanePoint() const override {
        return clippingPlanePoint;
    }

    void setClippingPlanePoint(glm::vec3 p) override {
        _triRenderer.setClippingPlanePoint(p);
        _pointSetRenderer.setClippingPlanePoint(p);
        clippingPlanePoint = p;
    }

    glm::vec3 getClippingPlaneNormal() const override {
        return clippingPlaneNormal;
    }

    void setClippingPlaneNormal(glm::vec3 n) override {
        _triRenderer.setClippingPlaneNormal(n);
        _pointSetRenderer.setClippingPlaneNormal(n);
        clippingPlaneNormal = n;
    }

    bool getInvertClipping() const override {
        return invertClipping;
    }

    void setInvertClipping(bool invert) override {
        _triRenderer.setInvertClipping(invert);
        _pointSetRenderer.setInvertClipping(invert);
        invertClipping = invert;
    }

    float getMeshSize() const override {
        return meshSize;
    }

    void setMeshSize(float val) override {
        _triRenderer.setMeshSize(val);
        meshSize = val;
    }

    float getMeshShrink() const override {
        return meshShrink;
    }

    void setMeshShrink(float val) override {
        _triRenderer.setMeshShrink(val);
        meshShrink = val;
    }

    bool getMeshVisible() const override {
        return _triRenderer.getVisible();
    }

    void setMeshVisible(bool visible) override {
        return _triRenderer.setVisible(visible);
    }

    int getFragRenderMode() const override {
        return fragRenderMode;
    }

    void setFragRenderMode(RenderMode mode) override {
        _triRenderer.setFragRenderMode(mode);
        fragRenderMode = mode;
    }

    int getSelectedColormap() const override {
        return selectedColormap;
    }

    void setSelectedColormap(int idx) override {
        _triRenderer.setSelectedColormap(idx);
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
        return 0;
        // return _halfedgeRenderer.getEdgeSize();
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
        _triRenderer.setMeshIndex(index);
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
        _triRenderer.setHighlight(idx, highlight);
    }

    void setHighlight(std::vector<float> highlights) override {
        _triRenderer.setHighlight(highlights);
    }

    void setPointHighlight(int idx, float highlight) override {
        _pointSetRenderer.setHighlight(idx, highlight);
    }

    void setPointHighlight(std::vector<float> highlights) override {
        _pointSetRenderer.setHighlight(highlights);
    }

    void setFilter(int idx, bool filter) override {

        _triRenderer.setFilter(idx, filter);
        Surface::Facet f(_tri, idx);
        for (int lv = 0; lv < 3; ++lv) {
            auto v = f.vertex(lv);

            bool allFiltered = true;

            // Iterate on corners
            for (int c = 0; c < _tri.facets.size(); ++c) {
                // If iterated corner is not current vertex, skip !
                if (_tri.facets[c] != v)
                    continue;

                int fi = c / 3;
                if (_triRenderer.getFilterPtr()[fi] <= 0) {
                    allFiltered = false;
                    break;
                }
            }

            // Only filter point when all attached facets are filtered
            _pointSetRenderer.setFilter(v, allFiltered);

        }
        
    }

    using Model::setSelectedAttr;
    void setSelectedAttr(int idx) override;

    private: 

    // Mesh
    Triangles _tri;
    SurfaceAttributes _surfaceAttributes;

    // Renderers
    // PointSetRenderer _pointSetRenderer;
    //HalfedgeRenderer _halfedgeRenderer;
    TriRenderer _triRenderer;

};