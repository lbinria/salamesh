#pragma once

#include "../../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"
#include "../renderers/bbox_renderer/bbox_renderer.h"
#include "../renderers/clipping_renderer.h"
#include "../layer.h"
#include "../helpers.h"

struct PolylineModel : public Model {

	PolylineModel() : 
		_m(),
		Model::Model({
			{"point_renderer", std::make_shared<PointSetRenderer>(_m.points) },
			{"edge_renderer", std::make_shared<PolylineRenderer>(_m)}, 
			{"bbox_renderer", std::make_shared<BBoxRenderer>(_m.points) },
			{"zclipping_renderer", std::make_shared<ClippingRenderer>(_m.points) }
		})
		{}

	ModelType getModelType() const override {
		return modelTypeFromMeshType<PolyLine>();
	}

	bool load(const std::string path) override {
		
		// Load the mesh
		try {
			_polylineAttributes = read_by_extension(path, _m);
		} catch (std::runtime_error &ex) {
			std::cerr << "Unable to read " << path << ": " << ex.what() << std::endl;
			return false;
		}

		_path = path;

		if (_m.nedges() <= 0)
			return false;

		clearAttrs();

		auto containers = getAttributeContainers();
		for (auto &[k, c] : containers) {
			addAttr(k, c);
		}

		init();
		push();

		return true;
	}

	bool saveAs(const std::string path, std::vector<Attribute> attrs) const override {
		// Check path validity
		if (path.empty()) {
			std::cerr << "Error: No path specified for saving the mesh." << std::endl;
			return false;
		}
		
		// Save attributes ! Convert back from salamesh attributes to NamedContainer vectors
		std::vector<NamedContainer> point_attrs;
		std::vector<NamedContainer> edge_attrs;
		for (auto &a : attrs) {
			// TODO do something more clear here !
			// Do not save attributes 
			// For example, attr : vec2 => attr[0], attr[1] aren't saved
			if (a.selectedDim != -1)
				continue;

			std::string name = a.name;
			ElementKind kind = a.kind;
			auto &container = a.ptr;

			if (kind == ElementKind::POINTS_ELT) {
				point_attrs.push_back(NamedContainer(name, container));
			} else if (kind == ElementKind::EDGES_ELT) {
				edge_attrs.push_back(NamedContainer(name, container));
			}
		}

		PolyLineAttributes attributes(
			point_attrs,
			edge_attrs
		);

		write_by_extension(path, _m, attributes);

		return true;
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

	PolyLine& getMesh() { return _m; }

	PolyLineAttributes& getSurfaceAttributes() { return _polylineAttributes; }
	const PolyLineAttributes& getSurfaceAttributes() const { return _polylineAttributes; }

	inline int nverts() const override {
		return _m.nverts();
	}

	inline int nfacets() const override {
		return 0;
	} 

	inline int ncells() const override {
		return 0;
	}

	inline int ncorners() const override {
		return 0;
	}

	inline int nhalfedges() const override {
		return _m.nedges();
	}

	long pickEdge(glm::vec3 p0, int f) {
		return 0;
		// auto &m = getSurface();

		// // Search nearest edge
		// double min_d = std::numeric_limits<double>().max();
		// long found_e = -1;
		
		// auto fc = m.facet(f);
		// int size = fc.size();

		// for (int lv = 0; lv < size; ++lv) {
			
		// 	// Get global indices of vertex on edge extremities
		// 	auto v0 = fc.vertex(lv % size);
		// 	auto v1 = fc.vertex((lv + 1) % size);

		// 	// Get points from current edge
		// 	vec3 p1 = m.points[v0];
		// 	vec3 p2 = m.points[v1];
		// 	vec3 b = (p1 + p2) * .5;
		// 	// Compute dist from picked point to bary of edge points
		// 	double d = (vec3(p0.x, p0.y, p0.z) - b).norm(); // TODO maybe use norm2 will give the same result

		// 	// Keep min dist
		// 	if (d < min_d) {
		// 		min_d = d;
		// 		found_e = f * size + lv;
		// 	}
		// }

		// return found_e;
	}

	protected:

	PolyLineAttributes _polylineAttributes;

	// virtual Surface& getSurface() = 0;
	// virtual const Surface& getSurface() const = 0;
	// Surface& getSurface() { return _m; }
	// const Surface& getSurface() const { return _m; }

	std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const override {
		std::vector<std::pair<ElementKind, NamedContainer>> containers;
		
		for (auto &c : _polylineAttributes.points)
			containers.push_back({ElementKind::POINTS_ELT, c});
		for (auto &c : _polylineAttributes.edges) {
			containers.push_back({ElementKind::EDGES_ELT, c});
		}
		return containers;
	}

	private:
	// Mesh
	PolyLine _m;
};
