#pragma once

#include "../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "point_set_renderer.h"
#include "halfedge_renderer_2.h"
#include "quad_renderer.h"
#include "bbox_renderer.h"
#include "clipping_renderer.h"

#include "color_mode.h"
#include "helpers.h"

using namespace UM;
using json = nlohmann::json;

struct QuadModel final : public Model {

	QuadModel() : 
		_m(), 
		Model::Model({
			{"mesh_renderer", std::make_shared<QuadRenderer>(_m)}, 
			{"point_renderer", std::make_shared<PointSetRenderer>(_m.points) },
			{"edge_renderer", std::make_shared<SurfaceHalfedgeRenderer>(_m) },
            {"bbox_renderer", std::make_shared<BBoxRenderer>(_m.points) },
            {"zclipping_renderer", std::make_shared<ClippingRenderer>(_m.points) }
		})
		{}


	ModelType getModelType() const override {
		return ModelType::QUAD;
	}

	bool load(const std::string path) override;
	void saveAs(const std::string path) const override;


	Quads& getQuads() { return _m; }
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

	// TODO implement this !!!
	void updateLayer(IRenderer::Layer layer) {

	}

	// TODO implements this !!!
	long pick_edge(glm::vec3 p0, int f) override {
		return -1;
	}

	private:

	// Mesh
	Quads _m;
	SurfaceAttributes _surfaceAttributes;

};