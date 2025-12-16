#pragma once

#include "../../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"
#include "../renderers/bbox_renderer.h"
#include "../renderers/poly_renderer.h"
#include "../renderers/bbox_renderer.h"
#include "../renderers/clipping_renderer.h"

#include "../color_mode.h"
#include "../layer.h"
#include "../helpers.h"

using namespace UM;
using json = nlohmann::json;

struct PolyModel final : public Model {

	PolyModel() : 
		_m(), 
		Model::Model({
			{"mesh_renderer", std::make_shared<PolyRenderer>(_m)}, 
			{"point_renderer", std::make_shared<PointSetRenderer>(_m.points) },
            {"edge_renderer", std::make_shared<SurfaceHalfedgeRenderer>(_m) },
            {"bbox_renderer", std::make_shared<BBoxRenderer>(_m.points) },
            {"zclipping_renderer", std::make_shared<ClippingRenderer>(_m.points) }
		})
		{}


	ModelType getModelType() const override {
		return ModelType::POLYGON_MODEL;
	}

	bool load(const std::string path) override;
	void saveAs(const std::string path) const override;


	Polygons& getPolygons() { return _m; }
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

	int nhalfedges() const override {
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

	// TODO implement this !!!
	void updateLayer(Layer layer, ElementKind kind) {

	}

    long pick_edge(glm::vec3 p0, int f) override {
        // Search nearest edge
        double min_d = std::numeric_limits<double>().max();
        long found_e = -1;
        
		auto fc = _m.facet(f);
		int size = fc.size();

        for (int lv = 0; lv < size; ++lv) {
            
        	// Get global indices of vertex on edge extremities
        	auto v0 = fc.vertex(lv % size);
        	auto v1 = fc.vertex((lv + 1) % size);

        	// Get points from current edge
        	vec3 p1 = _m.points[v0];
        	vec3 p2 = _m.points[v1];
        	vec3 b = (p1 + p2) * .5;
        	// Compute dist from picked point to bary of edge points
        	double d = (vec3(p0.x, p0.y, p0.z) - b).norm(); // TODO maybe use norm2 will give the same result

        	// Keep min dist
        	if (d < min_d) {
        		min_d = d;
        		found_e = f * size + lv;
        	}
        }

        return found_e;
    }

	private:

	// Mesh
	Polygons _m;
	SurfaceAttributes _surfaceAttributes;

};