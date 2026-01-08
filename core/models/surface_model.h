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

	void saveAs(const std::string path) const override;

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

	long pickEdge(glm::vec3 p0, int f) override {
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

	std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const override {
		std::vector<std::pair<ElementKind, NamedContainer>> containers;
		
		for (auto &c : _surfaceAttributes.points)
			containers.push_back({ElementKind::POINTS_ELT, c});
		for (auto &c : _surfaceAttributes.corners) {
			containers.push_back({ElementKind::CORNERS_ELT, c});
			containers.push_back({ElementKind::EDGES_ELT, c}); // TODO see pertinence
		}
		for (auto &c : _surfaceAttributes.facets)
			containers.push_back({ElementKind::FACETS_ELT, c});

		return containers;
	}

};