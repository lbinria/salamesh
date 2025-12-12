#pragma once

#include "../../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"
#include "../renderers/tri_renderer.h"
#include "../renderers/bbox_renderer.h"
#include "../renderers/clipping_renderer.h"
#include "../color_mode.h"
#include "../layer.h"
#include "../helpers.h"

using namespace UM;
using json = nlohmann::json;

struct TriModel final : public Model {

	TriModel() : 
        _m(), 
        Model::Model({
            {"mesh_renderer", std::make_shared<TriRenderer>(_m)}, 
            {"point_renderer", std::make_shared<PointSetRenderer>(_m.points) },
            {"edge_renderer", std::make_shared<SurfaceHalfedgeRenderer>(_m) },
            {"bbox_renderer", std::make_shared<BBoxRenderer>(_m.points) },
            {"zclipping_renderer", std::make_shared<ClippingRenderer>(_m.points) }
        })
        {}


    ModelType getModelType() const override {
        return ModelType::TRI;
    }


	bool load(const std::string path) override;
	void saveAs(const std::string path) const override;


	Triangles& getTriangles() { return _m; }
	SurfaceAttributes& getSurfaceAttributes() { return _surfaceAttributes; }

    int nverts() const override {
        return _m.nverts();
    }

    int nfacets() const override {
        return _m.nfacets();
    } 

    int ncells() const override {
        return 0;
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

    void updateLayer(Layer layer) {

        auto &selectedAttr = selectedAttrByLayer[layer];

        std::vector<float> data;

        switch (selectedAttr.elementKind) {
            case ElementKind::POINTS_ELT:
            {
                PointAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _m))
                    return;

                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::CORNERS_ELT: 
            {
                CornerAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _m))
                    return;

                data = layerAttr.ptr->data;
                break;
            }
            case ElementKind::FACETS_ELT:
            {
                FacetAttribute<float> layerAttr;
                if (!layerAttr.bind(selectedAttr.attrName, _surfaceAttributes, _m))
                    return;

                data = layerAttr.ptr->data;
                break;
            }
            default:
                std::cerr << "Warning: TriModel::updateLayer() on " 
                    << elementKindToString(selectedAttr.elementKind) 
                    << " with layer " 
                    << layerToString(layer) 
                    << " is not supported.." << std::endl;
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
        	auto v0 = _m.facet(f).vertex(lv % 3);
        	auto v1 = _m.facet(f).vertex((lv + 1) % 3);

        	// Get points from current edge
        	vec3 p1 = _m.points[v0];
        	vec3 p2 = _m.points[v1];
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
    Triangles _m;
    SurfaceAttributes _surfaceAttributes;

};