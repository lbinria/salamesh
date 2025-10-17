#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "tet_renderer.h"
#include "color_mode.h"
#include "helpers.h"


using namespace UM;
using json = nlohmann::json;

struct TetModel final : public Model {

	// Mesh + Renderer

    // using Model::Model;

	TetModel() : 
        _m(), 
        Model::Model({
            {"mesh_renderer", std::make_shared<TetRenderer>(_m)}, 
            {"point_renderer", std::make_shared<PointSetRenderer>(_m.points) }
        })
        {}

    ModelType getModelType() const override {
        return ModelType::TET;
    }


	bool load(const std::string path) override;
	// void save() const override;
	void saveAs(const std::string path) const override;

    // std::string save_state() const override;
    // void load_state(json model_state);

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

    int ncorners() const override {
        return _m.ncorners();
    }

    std::tuple<glm::vec3, glm::vec3> bbox() override {
        glm::vec3 min = glm::vec3(FLT_MAX);
        glm::vec3 max = glm::vec3(-FLT_MAX);

        for (auto &v : _m.iter_vertices()) {
            glm::vec3 p = sl::um2glm(v.pos());
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        return {min, max};
    }


    // // TODO filter anything else than cell !
    // void setFilter(int idx, bool filter) override {
    //     _meshRenderer->setFilter(idx, filter);

    //     // TODO it works but... not very efficient !
    //     Volume::Cell c(_m, idx);
    //     for (int lc = 0; lc < 8; ++lc) {
    //         auto corner = c.corner(lc);
    //         auto v = corner.vertex();
    //         // Retrieve all cells attached to this point to see whether filtered
    //         bool allFiltered = true;
    //         // #pragma omp parallel for
    //         for (int i = 0; i < _m.cells.size(); ++i) {
    //             if (_m.cells[i] != v)
    //                 continue;
                
    //             int ci = i / 8;
    //             if (_meshRenderer->getFilterPtr()[ci] <= 0) {
    //                 allFiltered = false;
    //                 break;
    //             }
    //         }

    //         _pointSetRenderer.setFilter(v, allFiltered);
    //         // _pointSetRenderer.setHighlight(v, 0.1f);
    //     }

        
    // }


    // TODO its the same as hex_model, refact !
    void updateLayer(IRenderer::Layer layer) {

        auto &selectedAttr = selectedAttrByLayer[layer];
        
        std::vector<float> data;

        switch (selectedAttr.elementKind) {
            case ElementKind::CELLS:
            {
                CellAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _volumeAttributes, _m))
                    return;
                
                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::CELL_FACETS:
            {
                CellFacetAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _volumeAttributes, _m))
                    return;
                
                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::POINTS_ELT:
            {
                PointAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _volumeAttributes, _m))
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
    }

    long pick_edge(glm::vec3 p0, int c) override {
        // Search nearest edge
        double min_d = std::numeric_limits<double>().max();
        long found_e = -1;
        
        for (int le = 0; le < 12; ++le) {
            long e = c * 12 + le;

            // Get local points indices of edge extremities
            int lv0 = reference_cells[0].facets[le % 3 + (le / 3 * 3)];
            int lv1 = reference_cells[0].facets[(le + 1) % 3 + (le / 3 * 3)];

            // Get global index of points
            int v0 = _m.vert(c, lv0);
            int v1 = _m.vert(c, lv1);

            // Get points from current edge
            vec3 p1 = _m.points[v0];
            vec3 p2 = _m.points[v1];
            vec3 b = (p1 + p2) * .5;
            // Compute dist from picked point to bary of edge points
            double d = (vec3(p0.x, p0.y, p0.z) - b).norm(); // TODO maybe use norm2 will give the same result

            // Keep min dist
            if (d < min_d) {
                min_d = d;
                found_e = e;
            }
        }

        return found_e;
    }

    private:

    // Mesh
    Tetrahedra _m;
    VolumeAttributes _volumeAttributes;

};