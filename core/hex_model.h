#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "hex_renderer.h"
#include "color_mode.h"

using namespace UM;
using json = nlohmann::json;

struct HexModel final : public Model {

	// Mesh + Renderer

    // using Model::Model;

	HexModel() : 
        _m(), 
        Model::Model(std::make_unique<HexRenderer>(_m), PointSetRenderer(_m.points),  std::make_shared<HalfedgeRenderer>(_m))
        {}

    ModelType getModelType() const override {
        return ModelType::HEX;
    }


	bool load(const std::string path) override;
	// void save() const override;
	void saveAs(const std::string path) const override;

    // std::string save_state() const override;
    // void load_state(json model_state);

	Hexahedra& getHexahedra() { return _m; }
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

    void updateHighlights() override {
        
        switch (selectedHighlightElement) {
            case ElementKind::CELLS:
            {
                CellAttribute<float> hl_c;
                if (!hl_c.bind(selectedHighlightAttr, _volumeAttributes, _m))
                    return;
                
                _meshRenderer->resizeHightlightBuffer(hl_c.ptr->data.size());
                _meshRenderer->setHighlight(hl_c.ptr->data);                

                break;
            }
            case ElementKind::CELL_FACETS:
            {
                CellFacetAttribute<float> hl_f;
                if (!hl_f.bind(selectedHighlightAttr, _volumeAttributes, _m))
                    return;

                _meshRenderer->resizeHightlightBuffer(hl_f.ptr->data.size());
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

    void updateFilters() override {
        switch (selectedFilterElement) {
            case ElementKind::CELLS:
            {
                CellAttribute<bool> flt_c;
                if (!flt_c.bind(selectedFilterAttr, _volumeAttributes, _m))
                    return;
                
                _meshRenderer->setFilter(flt_c.ptr->data);

                break;
            }
            case ElementKind::CELL_FACETS:
            {
                CellFacetAttribute<bool> flt_f;
                if (!flt_f.bind(selectedFilterAttr, _volumeAttributes, _m))
                    return;

                _meshRenderer->setFilter(flt_f.ptr->data);

                break;
            }
            case ElementKind::POINTS:
            {
                PointAttribute<bool> flt_p;
                if (!flt_p.bind(selectedFilterAttr, _volumeAttributes, _m))
                    return;

                _pointSetRenderer.setFilter(flt_p.ptr->data);

                break;
            }
            default:
                std::cerr << "Warning: HexModel::updateFilters() only supports filter on cells, cell facets or points." << std::endl;
                return;
        }
    }
    
    private:

    // Mesh
    Hexahedra _m;
    VolumeAttributes _volumeAttributes;
    
};