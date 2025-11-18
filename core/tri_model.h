#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer_2.h"
#include "tri_renderer.h"
#include "bbox_renderer.h"
#include "clipping_renderer.h"
#include "color_mode.h"
#include "helpers.h"

using namespace UM;
using json = nlohmann::json;

struct TriModel final : public Model {

	// Mesh + Renderer
    // using Model::Model;

	TriModel() : 
        _tri(), 
        Model::Model({
            {"mesh_renderer", std::make_shared<TriRenderer>(_tri)}, 
            {"point_renderer", std::make_shared<PointSetRenderer>(&_tri) },
            {"edge_renderer", std::make_shared<HalfedgeRenderer2>(_tri) },
            {"bbox_renderer", std::make_shared<BBoxRenderer>(_tri.points) },
            {"zclipping_renderer", std::make_shared<ClippingRenderer>(_tri.points) }
        })
        {}


    ModelType getModelType() const override {
        return ModelType::TRI;
    }


	bool load(const std::string path) override;
	void saveAs(const std::string path) const override;


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

    std::tuple<glm::vec3, glm::vec3> bbox() override {
        glm::vec3 min = glm::vec3(FLT_MAX);
        glm::vec3 max = glm::vec3(-FLT_MAX);

        for (auto &v : _tri.iter_vertices()) {
            glm::vec3 p = sl::um2glm(v.pos());
            min = glm::min(min, p);
            max = glm::max(max, p);
        }

        return {min, max};
    }

    void updateLayer(IRenderer::Layer layer) {

        auto &selectedAttr = selectedAttrByLayer[layer];

        std::vector<float> data;

        switch (selectedAttr.elementKind) {
            case ElementKind::POINTS_ELT:
            {
                PointAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _tri))
                    return;

                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::CORNERS: 
            {
                CornerAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _tri))
                    return;

                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::FACETS:
            {
                FacetAttribute<float> layerAttr;
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
    }

    long pick_edge(glm::vec3 p0, int f) override {
        // Search nearest edge
        double min_d = std::numeric_limits<double>().max();
        long found_e = -1;
        
        for (int lv = 0; lv < 3; ++lv) {
            
        	// Get global indices of vertex on edge extremities
        	auto v0 = _tri.facet(f).vertex(lv % 3);
        	auto v1 = _tri.facet(f).vertex((lv + 1) % 3);

        	// Get points from current edge
        	vec3 p1 = _tri.points[v0];
        	vec3 p2 = _tri.points[v1];
        	vec3 b = (p1 + p2) * .5;
        	// Compute dist from picked point to bary of edge points
        	double d = (vec3(p0.x, p0.y, p0.z) - b).norm(); // TODO maybe use norm2 will give the same result

        	// Keep min dist
        	if (d < min_d) {
        		min_d = d;
        		found_e = f * 3 + lv;
        	}
        }

        return found_e;
    }

    private: 

    // Mesh
    Triangles _tri;
    SurfaceAttributes _surfaceAttributes;

};