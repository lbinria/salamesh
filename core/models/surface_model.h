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

struct SurfaceModel : public Model {

	SurfaceModel(std::map<std::string, std::shared_ptr<IRenderer>> renderers) :
	Model::Model(renderers) {}

	SurfaceModel(std::map<std::string, std::shared_ptr<IRenderer>> renderers, std::string name) : 
	Model::Model(renderers, name) {}

	std::tuple<glm::vec3, glm::vec3> bbox() override {
		auto &m = getSurface();

		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);

		for (auto &v : m.iter_vertices()) {
			glm::vec3 p = sl::um2glm(v.pos());
			min = glm::min(min, p);
			max = glm::max(max, p);
		}

		return {min, max};
	}

	SurfaceAttributes& getSurfaceAttributes() { return _surfaceAttributes; }
	const SurfaceAttributes& getSurfaceAttributes() const { return _surfaceAttributes; }

	int nverts() const override {
		auto &m = getSurface();
		return m.nverts();
	}

	int nfacets() const override {
		auto &m = getSurface();
		return m.nfacets();
	} 

	int ncells() const override {
		return 0;
	}

	int ncorners() const override {
		auto &m = getSurface();
		return m.ncorners();
	}

	int nhalfedges() const override {
		auto &m = getSurface();
		return m.ncorners();
	}

	long pick_edge(glm::vec3 p0, int f) override {
		auto &m = getSurface();

		// Search nearest edge
		double min_d = std::numeric_limits<double>().max();
		long found_e = -1;
		
		auto fc = m.facet(f);
		int size = fc.size();

		for (int lv = 0; lv < size; ++lv) {
			
			// Get global indices of vertex on edge extremities
			auto v0 = fc.vertex(lv % size);
			auto v1 = fc.vertex((lv + 1) % size);

			// Get points from current edge
			vec3 p1 = m.points[v0];
			vec3 p2 = m.points[v1];
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

	protected:

	SurfaceAttributes _surfaceAttributes;

	virtual Surface& getSurface() = 0;
	virtual const Surface& getSurface() const = 0;

	// TODO be able to have other attribute types than double
	// TODO move data recuperation into a function in model.h to move data range set
	void updateLayer(Layer layer, ElementKind kind) override {

		auto attrName = getLayerAttr(layer, kind);

		std::optional<std::vector<double>> data_opt;

		switch (kind) {
			case ElementKind::POINTS_ELT: {
				data_opt = getAttrData<PointAttribute<double>>(attrName);
				break;
			}
			case ElementKind::CORNERS_ELT:
			case ElementKind::EDGES_ELT: {
				data_opt = getAttrData<CornerAttribute<double>>(attrName);
				break;
			}
			case ElementKind::FACETS_ELT: {
				data_opt = getAttrData<FacetAttribute<double>>(attrName);
				break;
			}
			default:
				std::cerr << "Warning: Model::updateLayer() on " 
					<< elementKindToString(kind) 
					<< " of "
					<< modelTypeToString(getModelType())
					<< " with layer " 
					<< layerToString(layer) 
					<< " is not supported.." << std::endl;
				return;
		}

		if (!data_opt.has_value())
			return;

		// double to float
		std::vector<float> data(data_opt.value().size());
		std::transform(data_opt.value().begin(), data_opt.value().end(), data.begin(), [](auto v) { return static_cast<float>(v); });

		auto [min, max] = sl::getRange(data);

		for (auto const &[k, r] : _renderers) {
			if (r->isRenderElement(kind)) {
				r->setLayerRange(layer, min, max);
				r->setLayer(data, layer);
			}
		}
	}



	private:

	template<typename T> 
	std::optional<std::vector<double>> getAttrData(std::string attrName) {
		T layerAttr;
		if (!layerAttr.bind(attrName, _surfaceAttributes, getSurface()))
			return std::nullopt;

		return layerAttr.ptr->data;
	}

};