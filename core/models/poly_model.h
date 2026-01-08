#pragma once

#include "../../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "surface_model.h"
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

struct PolyModel final : public SurfaceModel {

	PolyModel() : 
		_m(), 
		SurfaceModel::SurfaceModel({
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

	Polygons& getPolygons() { return _m; }
	Surface& getSurface() override { return _m; }
	const Surface& getSurface() const override { return _m; }

	private:

	// Mesh
	Polygons _m;

};