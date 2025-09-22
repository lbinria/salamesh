#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "quad_renderer.h"
#include "color_mode.h"
#include "render_mode.h"

using namespace UM;
using json = nlohmann::json;

struct QuadModel final : public Model {

	// Mesh + Renderer
    // using Model::Model;

	QuadModel() : 
        _quad(), 
        // _quadRenderer(_quad), 
        Model::Model(std::make_unique<QuadRenderer>(_quad), PointSetRenderer(_quad.points), std::nullopt)
        // _pointSetRenderer(_quad.points)
        /* _halfedgeRenderer(_quad)*/ {}


    ModelType getModelType() const override {
        return ModelType::QUAD;
    }


	bool load(const std::string path) override;
	// void save() const override;
	void saveAs(const std::string path) const override;


	// std::string save_state() const override;
	// void load_state(json model_state);


	Quads& getQuads() { return _quad; }
	SurfaceAttributes& getSurfaceAttributes() { return _surfaceAttributes; }

    int nverts() const override {
        return _quad.nverts();
    }

    int nfacets() const override {
        return _quad.nfacets();
    } 

    int ncells() const override {
        return 0;
    } 

    void setFilter(int idx, bool filter) override {

        _meshRenderer->setFilter(idx, filter);
        Surface::Facet f(_quad, idx);
        for (int lv = 0; lv < 3; ++lv) {
            auto v = f.vertex(lv);

            bool allFiltered = true;

            // Iterate on corners
            for (int c = 0; c < _quad.facets.size(); ++c) {
                // If iterated corner is not current vertex, skip !
                if (_quad.facets[c] != v)
                    continue;

                int fi = c / 3;
                if (_meshRenderer->getFilterPtr()[fi] <= 0) {
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
    Quads _quad;
    SurfaceAttributes _surfaceAttributes;

    // Renderers
    // PointSetRenderer _pointSetRenderer;
    //HalfedgeRenderer _halfedgeRenderer;
    // QuadRenderer _quadRenderer;

    // template<typename T>
    // Attribute bindAttr(std::string name, ElementKind kind) {
    //     switch (kind) {
    //         case ElementKind::POINTS:
    //             PointAttribute<T> a;
    //             a.bind(name, _surfaceAttributes, _quad);
    //             return bindAttr<T>(name, a);
    //         case ElementKind::EDGES:
    //             EdgeAttribute<T> a;
    //             a.bind(name, _surfaceAttributes, _quad);
    //             return bindAttr<T>(name, a);
    //         case ElementKind::FACETS:
    //             FacetAttribute<T> a;
    //             a.bind(name, _surfaceAttributes, _quad);
    //             return bindAttr<T>(name, a);
    //         case ElementKind::CELL_FACETS:
    //             CellFacetAttribute<T> a;
    //             a.bind(name, _surfaceAttributes, _quad);
    //             return bindAttr<T>(name, a);
    //         case ElementKind::CORNERS:
    //             CornerAttribute<T> a;
    //             a.bind(name, _surfaceAttributes, _quad);
    //             return bindAttr<T>(name, a);
    //         case ElementKind::CELL_CORNERS:
    //             CellCornerAttribute<T> a;
    //             a.bind(name, _surfaceAttributes, _quad);
    //             return bindAttr<T>(name, a);
    //         case ElementKind::CELLS:
    //             CellAttribute<T> a;
    //             a.bind(name, _surfaceAttributes, _quad);
    //             return bindAttr<T>(name, a);
    //         default:
    //             throw std::runtime_error("Unknown element kind for binding attribute: " + elementKindToString(kind));
    //     }
    // }


};