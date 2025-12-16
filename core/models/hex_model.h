#pragma once

#include "../../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"
#include "../renderers/hex_renderer.h"
#include "../renderers/bbox_renderer.h"
#include "../renderers/clipping_renderer.h"

#include "../color_mode.h"
#include "../layer.h"
#include "../helpers.h"

using namespace UM;
using json = nlohmann::json;

struct HexModel final : public Model {

	HexModel() : 
        _m(), 
        Model::Model({
            {"mesh_renderer", std::make_shared<HexRenderer>(_m)}, 
            {"point_renderer", std::make_shared<PointSetRenderer>(_m.points) },
            {"edge_renderer", std::make_shared<VolumeHalfedgeRenderer>(_m) },
            {"bbox_renderer", std::make_shared<BBoxRenderer>(_m.points) },
            {"zclipping_renderer", std::make_shared<ClippingRenderer>(_m.points) }
        })
        {}

    ModelType getModelType() const override {
        return ModelType::HEX_MESH;
    }


	bool load(const std::string path) override;
	void saveAs(const std::string path) const override;

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

    int ncorners() const override {
        return _m.ncorners();
    }

    int nhalfedges() const override {
        return _m.ncells() * 24;
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

    // TODO its the same as tet_model, refact !
    void updateLayer(Layer layer, ElementKind kind) {

        auto attrName = getLayerAttr(layer, kind);
        
        std::vector<float> data;

        switch (kind) {
            case ElementKind::CELLS_ELT:
            {
                CellAttribute<float> layerAttr;
                if (!layerAttr.bind(attrName, _volumeAttributes, _m))
                    return;
                
                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::CELL_FACETS_ELT:
            {
                CellFacetAttribute<float> layerAttr;
                if (!layerAttr.bind(attrName, _volumeAttributes, _m))
                    return;
                
                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::POINTS_ELT:
            {
                PointAttribute<float> layerAttr;
                if (!layerAttr.bind(attrName, _volumeAttributes, _m))
                    return;
                
                data = layerAttr.ptr->data;
                break;
            }
            default:
                std::cerr << "Warning: HexModel::updateLayer() on " 
                    << elementKindToString(kind) 
                    << " with layer " 
                    << layerToString(layer) 
                    << " is not supported.." << std::endl;
                return;
        }

        for (auto const &[k, r] : _renderers) {
            if (r->isRenderElement(kind)) {
                r->setLayer(data, layer);
            }
        }
    }

    long pick_edge(glm::vec3 p0, int c) override {
		// Search nearest edge
		double min_d = std::numeric_limits<double>().max();
		long found_e = -1;
		
		for (int le = 0; le < 24; ++le) {
			long e = c * 24 + le;

			// Get local points indices of edge extremities
			int lv0 = reference_cells[1].facets[le % 4 + (le / 4 * 4)];
			int lv1 = reference_cells[1].facets[(le + 1) % 4 + (le / 4 * 4)];

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
    Hexahedra _m;
    VolumeAttributes _volumeAttributes;
    
};