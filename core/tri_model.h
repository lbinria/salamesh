#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "tri_renderer.h"
#include "color_mode.h"

using namespace UM;
using json = nlohmann::json;

struct TriModel final : public Model {

	// Mesh + Renderer
    // using Model::Model;

	TriModel() : 
        _tri(), 
        Model::Model({
            {"mesh_renderer", std::make_shared<TriRenderer>(_tri)}, 
            {"point_renderer", std::make_shared<PointSetRenderer>(_tri.points) }
        })
        {}


    ModelType getModelType() const override {
        return ModelType::TRI;
    }


	bool load(const std::string path) override;
	// void save() const override;
	void saveAs(const std::string path) const override;


	// std::string save_state() const override;
	// void load_state(json model_state);


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

    int ncorners() const override {
        return _tri.ncorners();
    }

    // void setFilter(int idx, bool filter) override {

    //     _meshRenderer->setFilter(idx, filter);
    //     Surface::Facet f(_tri, idx);
    //     for (int lv = 0; lv < 3; ++lv) {
    //         auto v = f.vertex(lv);

    //         bool allFiltered = true;

    //         // Iterate on corners
    //         for (int c = 0; c < _tri.facets.size(); ++c) {
    //             // If iterated corner is not current vertex, skip !
    //             if (_tri.facets[c] != v)
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

        auto &selectedAttr = selectedAttrByLayer[layer];

        std::vector<float> data;

        switch (selectedAttr.elementKind) {
            case ElementKind::FACETS:
            {
                FacetAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _tri))
                    return;

                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::POINTS:
            {
                PointAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _tri))
                    return;

                data = layerAttr.ptr->data;
                break;
            }
            default:
                std::cerr << "Warning: HexModel::updateFilters() only supports highlight on cells, cell facets or points." << std::endl;
                return;
        }

        for (auto const &[k, r] : _renderers) {
            if (r->isRenderElement(selectedAttr.elementKind)) {
                r->setLayer(data, layer);
            }
        }
        


        // switch (selectedAttr.elementKind) {
        //     case ElementKind::FACETS:
        //     {
        //         FacetAttribute<float> layerAttr;
        //         if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _tri))
        //             return;

        //         _meshRenderer->setLayer(layerAttr.ptr->data, layer);

        //         break;
        //     }
        //     case ElementKind::POINTS:
        //     {
        //         PointAttribute<float> layerAttr;
        //         if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _tri))
        //             return;

        //         _pointSetRenderer.setLayer(layerAttr.ptr->data, layer);

        //         break;
        //     }
        //     default:
        //         std::cerr << "Warning: HexModel::updateFilters() only supports highlight on cells, cell facets or points." << std::endl;
        //         return;
        // }
    }

    private: 

    // Mesh
    Triangles _tri;
    SurfaceAttributes _surfaceAttributes;

};