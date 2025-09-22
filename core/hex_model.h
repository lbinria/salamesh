#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer.h"
#include "hex_renderer.h"
#include "color_mode.h"
#include "render_mode.h"

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
    
    private:

    // Mesh
    Hexahedra _m;
    VolumeAttributes _volumeAttributes;
    
};