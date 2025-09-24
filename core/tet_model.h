#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "tet_renderer.h"
#include "color_mode.h"

using namespace UM;
using json = nlohmann::json;

struct TetModel final : public Model {

	// Mesh + Renderer

    // using Model::Model;

	TetModel() : 
        _m(), 
        Model::Model(std::make_unique<TetRenderer>(_m), PointSetRenderer(_m.points), nullptr) {
        }

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

    // TODO filter anything else than cell !
    void setFilter(int idx, bool filter) override {
        _meshRenderer->setFilter(idx, filter);

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
                if (_meshRenderer->getFilterPtr()[ci] <= 0) {
                    allFiltered = false;
                    break;
                }
            }

            _pointSetRenderer.setFilter(v, allFiltered);
            // _pointSetRenderer.setHighlight(v, 0.1f);
        }

        
    }

    // The same as HexModel
    void updateHighlights() override {
        
        switch (selectedHighlightElement) {
            case ElementKind::CELLS:
            {
                CellAttribute<float> hl_c;
                if (!hl_c.bind(selectedHighlightAttr, _volumeAttributes, _m))
                    return;
                
                // _meshRenderer->resizeHightlightBuffer(hl_c.ptr->data.size());
                _meshRenderer->setHighlight(hl_c.ptr->data);                

                break;
            }
            case ElementKind::CELL_FACETS:
            {
                CellFacetAttribute<float> hl_f;
                if (!hl_f.bind(selectedHighlightAttr, _volumeAttributes, _m))
                    return;

                // _meshRenderer->resizeHightlightBuffer(hl_f.ptr->data.size());
                _meshRenderer->setHighlight(hl_f.ptr->data);

                break;
            }
            case ElementKind::POINTS:
            {
                PointAttribute<float> hl_p;
                if (!hl_p.bind(selectedHighlightAttr, _volumeAttributes, _m))
                    return;

                // _pointSetRenderer.resizeHightlightBuffer(hl_p.ptr->data.size());
                _pointSetRenderer.setHighlight(hl_p.ptr->data);

                break;
            }
            default:
                std::cerr << "Warning: HexModel::updateHighlights() only supports highlight on cells, cell facets or points." << std::endl;
                return;
        }

    }

    // The same as HexModel
    void updateFilters() override {
        
        switch (selectedHighlightElement) {
            case ElementKind::CELLS:
            {
                CellAttribute<bool> filter;
                if (!filter.bind(selectedFilterAttr, _volumeAttributes, _m))
                    return;

                // Convert data bool -> float
                std::vector<float> f_filters(filter.ptr->data.size());
                std::transform(filter.ptr->data.begin(), filter.ptr->data.end(), f_filters.begin(), [](bool v) { return v ? 1.f : 0.f; });
                
                _meshRenderer->setFilter(f_filters);

                break;
            }
            case ElementKind::CELL_FACETS:
            {
                CellFacetAttribute<bool> filter;
                if (!filter.bind(selectedFilterAttr, _volumeAttributes, _m))
                    return;

                // Convert data bool -> float
                std::vector<float> f_filters(filter.ptr->data.size());
                std::transform(filter.ptr->data.begin(), filter.ptr->data.end(), f_filters.begin(), [](bool v) { return v ? 1.f : 0.f; });

                _meshRenderer->setFilter(f_filters);

                break;
            }
            case ElementKind::POINTS:
            {
                PointAttribute<bool> filter;
                if (!filter.bind(selectedFilterAttr, _volumeAttributes, _m))
                    return;

                // Convert data bool -> float
                std::vector<float> f_filters(filter.ptr->data.size());
                std::transform(filter.ptr->data.begin(), filter.ptr->data.end(), f_filters.begin(), [](bool v) { return v ? 1.f : 0.f; });

                _pointSetRenderer.setFilter(f_filters);

                break;
            }
            default:
                std::cerr << "Warning: HexModel::updateFilters() only supports highlight on cells, cell facets or points." << std::endl;
                return;
        }

    }

    private:

    // Mesh
    Tetrahedra _m;
    VolumeAttributes _volumeAttributes;

};