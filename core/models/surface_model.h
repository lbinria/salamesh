#pragma once

#include "../../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"
#include "../renderers/tri_renderer.h"
#include "../renderers/poly_renderer.h"
#include "../renderers/surface_renderer2.h"
#include "../renderers/bbox_renderer/bbox_renderer.h"
#include "../renderers/clipping_renderer.h"
#include "../layer.h"
#include "../helpers.h"

// Define concept to accept only types that are derived from Surface
template<typename T>
concept SurfaceDerived = std::is_base_of_v<Surface, std::remove_cv_t<T>> && 
	!std::is_same_v<Surface, std::remove_cv_t<T>>;

// This code allow selection of the Renderer type according to a given Surface type
// It looks like to module functor in ocaml
namespace RendererSpecialization {
	template<typename T>
	struct RendererSelector;

	template<>
	struct RendererSelector<UM::Triangles> {
		using type = TriRenderer;
		// using type = TriRenderer2;
	};

	template<>
	struct RendererSelector<UM::Quads> {
		using type = PolyRenderer;
		// using type = QuadRenderer2;
	};

	template<>
	struct RendererSelector<UM::Polygons> {
		using type = PolyRenderer;
		// using type = PolyRenderer2;
	};
}

struct SurfModel : public Model {

	SurfModel(std::string name, std::map<std::string, std::shared_ptr<Renderer>> renderers) : Model::Model(name, renderers) {}

	SurfaceAttributes& getSurfaceAttributes() { return _surfaceAttributes; }
	const SurfaceAttributes& getSurfaceAttributes() const { return _surfaceAttributes; }

	protected:
	SurfaceAttributes _surfaceAttributes;

};

template<SurfaceDerived TSurface>
struct SurfaceModel : public SurfModel {

	SurfaceModel(std::string name) : 
		_m(),
		SurfModel::SurfModel(name, {
			{"mesh_renderer", std::make_shared<typename RendererSpecialization::RendererSelector<TSurface>::type>("", _m)}, 
			{"point_renderer", std::make_shared<PointSetRenderer>("", _m.points) },
			{"edge_renderer", std::make_shared<SurfaceHalfedgeRenderer>("", _m) },
			{"bbox_renderer", std::make_shared<BBoxRenderer>("", _m.points) },
			{"zclipping_renderer", std::make_shared<ClippingRenderer>("", _m.points) }
		})
		{
			getPointsRenderer().setVisible(false);
			getEdgesRenderer()->setVisible(false);
		}

	ModelType getModelType() const override {
		return modelTypeFromMeshType<TSurface>();
	}

	int getDim() const override {
		return modelDimFromModelType<modelTypeFromMeshType<TSurface>()>();
	}

	bool load(const std::string path) override {
		
		// Load the mesh
		try {
			_surfaceAttributes = read_by_extension(path, _m);
		} catch (std::runtime_error &ex) {
			std::cerr << "Unable to read " << path << ": " << ex.what() << std::endl;
			return false;
		}

		_path = path;

		if (_m.nfacets() <= 0)
			return false;

		// We can load Triangles / Quads as Polygons, it works, i know
		// but there is maybe some optimisations for Triangles, Quads...
		// Note: check this improve the loading time of big models
		if (getModelType() == ModelType::POLYGON_MODEL && !mustLoadedAsPoly()) {
			return false;
		}

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
		std::vector<NamedContainer> facet_attrs;
		std::vector<NamedContainer> corner_attrs;
		for (auto &a : attrs) {
			// Do not save splitted attributes 
			// For example, attr : vec2 => attr[0], attr[1] aren't saved
			if (a.isSplit)
				continue;

			std::string name = a.name;
			ElementKind kind = a.kind;
			auto &container = a.ptr;

			if (kind == ElementKind::POINTS_ELT) {
				point_attrs.push_back(NamedContainer(name, container));
			} else if (kind == ElementKind::FACETS_ELT) {
				facet_attrs.push_back(NamedContainer(name, container));
			} else if (kind == ElementKind::CORNERS_ELT) {
				corner_attrs.push_back(NamedContainer(name, container));
			}
		}

		SurfaceAttributes attributes(
			point_attrs,
			facet_attrs,
			corner_attrs
		);

		write_by_extension(path, getSurface(), attributes);

		return true;
	}

	std::tuple<glm::vec3, glm::vec3> bbox() override {
		auto &m = getSurface();

		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);

		for (auto &v : m.iter_vertices()) {
			glm::vec3 p = sl::um2glm(v);
			min = glm::min(min, p);
			max = glm::max(max, p);
		}

		return {min, max};
	}

	Attribute bindAttr(std::string attrName, ElementKind kind, ElementType type) override {
		if (kind == ElementKind::POINTS_ELT) {
			switch (type)
			{
			case ElementType::DOUBLE_ELT: {
				PointAttribute<double> a;
				a.bind(attrName, _surfaceAttributes, _m);
				break;
			}
			case ElementType::INT_ELT: {
				PointAttribute<int> a;
				a.bind(attrName, _surfaceAttributes, _m);
				break;
			}
			case ElementType::BOOL_ELT: {
				PointAttribute<bool> a;
				a.bind(attrName, _surfaceAttributes, _m);
				break;
			}
			case ElementType::VEC2_ELT: {
				PointAttribute<vec2> a;
				a.bind(attrName, _surfaceAttributes, _m);
				break;
			}
			case ElementType::VEC3_ELT: {
				PointAttribute<vec3> a;
				a.bind(attrName, _surfaceAttributes, _m);
				break;
			}
			default:
				break;
			}
		} else if (kind == ElementKind::FACETS_ELT) {

		} else if (kind == ElementKind::CORNERS_ELT) {
			// blabla
		}


		auto container = getAttributeContainer(attrName, kind);
		return Attribute{attrName, kind, type, container.ptr};
	}

	TSurface& getMesh() { return _m; }
	Surface& getSurface() { return _m; }
	const Surface& getSurface() const { return _m; }



	inline int nverts() const override {
		return _m.nverts();
	}

	inline int nfacets() const override {
		return _m.nfacets();
	} 

	inline int ncells() const override {
		return 0;
	}

	inline int ncorners() const override {
		return _m.ncorners();
	}

	inline int nhalfedges() const override {
		return _m.ncorners();
	}

	inline void vert(int fi, int lv, int vi) override {
		_m.vert(fi, lv) = vi;
	}

	int createElements(int n, int size = 0) override {
		if constexpr (!std::is_same_v<Polygons, TSurface>) {
			return _m.create_facets(n);
		} else {
			return _m.create_facets(n, size);;
		}
	}

	void deleteElements(std::vector<int> indexes) override {
		std::vector<bool> toKill(_m.nfacets(), false);
		for (int i : indexes)
			toKill[i] = true;
		
		_m.delete_facets(toKill);
	}

	void deleteIsolatedVertices() override {
		_m.delete_isolated_vertices();
	}

	void deleteVertices(std::vector<int> indexes) {
		std::vector<bool> toKill(_m.nverts(), false);
		for (int i : indexes)
			toKill[i] = true;

		_m.delete_vertices(toKill);
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


	// virtual Surface& getSurface() = 0;
	// virtual const Surface& getSurface() const = 0;
	// Surface& getSurface() { return _m; }
	// const Surface& getSurface() const { return _m; }

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

	private:
	// Mesh
	TSurface _m;


	// Check whether mesh has more than 4 vertices per facet
	// if not, check that it contains irregular number of vertices per facet
	bool mustLoadedAsPoly() {
		if (_m.nfacets() <= 0)
			return true;

		int fs = _m.facet(0).size();

		// N vert per facet greater than quad
		if (fs > 4)
			return true;

		// Or irregular number of vertices
		for (auto &f : _m.iter_facets()) {
			if (fs != f.size())
				return true;
		}

		return false;
	}

};

typedef SurfaceModel<Triangles> TriModel;
typedef SurfaceModel<Quads> QuadModel;
typedef SurfaceModel<Polygons> PolyModel;
