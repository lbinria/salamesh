#pragma once

#include "../../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "surface_model.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"
#include "../renderers/quad_renderer.h"
#include "../renderers/bbox_renderer.h"
#include "../renderers/clipping_renderer.h"

#include "../color_mode.h"
#include "../layer.h"
#include "../helpers.h"

using namespace UM;
using json = nlohmann::json;

struct QuadModel final : public SurfaceModel {

	QuadModel() : 
		_m(), 
		SurfaceModel::SurfaceModel({
			{"mesh_renderer", std::make_shared<QuadRenderer>(_m)}, 
			{"point_renderer", std::make_shared<PointSetRenderer>(_m.points) },
			{"edge_renderer", std::make_shared<SurfaceHalfedgeRenderer>(_m) },
			{"bbox_renderer", std::make_shared<BBoxRenderer>(_m.points) },
			{"zclipping_renderer", std::make_shared<ClippingRenderer>(_m.points) }
		})
		{}


	ModelType getModelType() const override {
		return ModelType::QUAD_MODEL;
	}

	bool load(const std::string path) override;

	Quads& getQuads() { return _m; }
	Surface& getSurface() override { return _m; }
	const Surface& getSurface() const override { return _m; }

	private:

	// Mesh
	Quads _m;

};