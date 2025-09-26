#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "quad_renderer.h"
#include "color_mode.h"

using namespace UM;
using json = nlohmann::json;

struct QuadModel final : public Model {

	// Mesh + Renderer
    // using Model::Model;

	QuadModel() : 
        _quad(), 
        Model::Model({
            {"mesh_renderer", std::make_shared<QuadRenderer>(_quad)}, 
            {"point_renderer", std::make_shared<PointSetRenderer>(_quad.points) }
        })
        {}


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

    int ncorners() const override {
        return _quad.ncorners();
    }

    // void setFilter(int idx, bool filter) override {

    //     _meshRenderer->setFilter(idx, filter);
    //     Surface::Facet f(_quad, idx);
    //     for (int lv = 0; lv < 3; ++lv) {
    //         auto v = f.vertex(lv);

    //         bool allFiltered = true;

    //         // Iterate on corners
    //         for (int c = 0; c < _quad.facets.size(); ++c) {
    //             // If iterated corner is not current vertex, skip !
    //             if (_quad.facets[c] != v)
    //                 continue;

    //             int fi = c / 3;
    //             if (_meshRenderer->getFilterPtr()[fi] <= 0) {
    //                 allFiltered = false;
    //                 break;
    //             }
    //         }

    //         // Only filter point when all attached facets are filtered
    //         _pointSetRenderer.setFilter(v, allFiltered);

    //     }
        
    // }

    void updateLayer(IRenderer::Layer layer) {

    }

    private: 

    // Mesh
    Quads _quad;
    SurfaceAttributes _surfaceAttributes;

};