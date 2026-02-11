#pragma once

#include "../../include/json.hpp"
#include <ultimaille/all.h>
#include <string>
#include "model.h"
#include "../renderers/point_set_renderer.h"
#include "../renderers/halfedge_renderer.h"
#include "../renderers/tet_renderer.h"
#include "../renderers/hex_renderer.h"
#include "../renderers/bbox_renderer/bbox_renderer.h"
#include "../renderers/clipping_renderer.h"
#include "../layer.h"
#include "../helpers.h"

// Define concept to accept only types that are derived from Volume
template<typename T>
concept VolumeDerived = std::is_base_of_v<Volume, std::remove_cv_t<T>> && 
	!std::is_same_v<Volume, std::remove_cv_t<T>>;

// This code allow selection of the Renderer type according to a given Volume type
// It looks like to module functor in ocaml
namespace RendererSpecialization {
	template<typename T>
	struct RendererSelector;

	template<>
	struct RendererSelector<UM::Tetrahedra> {
		using type = TetRenderer;
	};

	template<>
	struct RendererSelector<UM::Hexahedra> {
		using type = HexRenderer;
	};

}

template<VolumeDerived TVolume>
struct VolumeModel final : public Model {

	VolumeModel() : 
		_m(), 
		Model::Model({
			{"mesh_renderer", std::make_shared<typename RendererSpecialization::RendererSelector<TVolume>::type>(_m)}, 
			{"point_renderer", std::make_shared<PointSetRenderer>(_m.points) },
			{"edge_renderer", std::make_shared<VolumeHalfedgeRenderer>(_m) },
			{"bbox_renderer", std::make_shared<BBoxRenderer>(_m.points) },
			{"zclipping_renderer", std::make_shared<ClippingRenderer>(_m.points) }
		})
		{}

	ModelType getModelType() const override {
		return modelTypeFromMeshType<TVolume>();
	}

	bool load(const std::string path) override {
		
		// Load the mesh
		try {
			_volumeAttributes = read_by_extension(path, _m);
		} catch (std::runtime_error &ex) {
			std::cerr << "Unable to read " << path << ": " << ex.what() << std::endl;
			return false;
		}

		_path = path;

		if (_m.ncells() <= 0)
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
		std::vector<NamedContainer> cell_corner_attrs;
		std::vector<NamedContainer> cell_facet_attrs;
		std::vector<NamedContainer> cell_attrs;
		for (auto &a : attrs) {
			std::string name = a.name;
			ElementKind kind = a.kind;
			auto &container = a.ptr;

			if (kind == ElementKind::POINTS_ELT) {
				point_attrs.push_back(NamedContainer(name, container));
			} else if (kind == ElementKind::CELL_CORNERS_ELT) {
				cell_corner_attrs.push_back(NamedContainer(name, container));
			} else if (kind == ElementKind::CELL_FACETS_ELT) {
				cell_facet_attrs.push_back(NamedContainer(name, container));
			} else if (kind == ElementKind::CELLS_ELT) {
				cell_attrs.push_back(NamedContainer(name, container));
			}
		}

		VolumeAttributes attributes(
			point_attrs,
			cell_attrs,
			cell_facet_attrs,
			cell_corner_attrs
		);

		write_by_extension(path, _m, attributes);

		return true;
	}

	TVolume& getMesh() { return _m; }

	VolumeAttributes& getVolumeAttributes() { return _volumeAttributes; }
	const VolumeAttributes& getVolumeAttributes() const { return _volumeAttributes; }

	int nverts() const override {
		return _m.nverts();
	}

	int nfacets() const override {
		return _m.nfacets();
	}

	int ncells() const override {
		return _m.ncells();
	}

	int ncorners() const override {
		return _m.ncorners();
	}

	int nhalfedges() const override {
		return _m.ncells() * 24;
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

	long pickEdge(glm::vec3 p0, int c) override {
		// Search nearest edge
		double min_d = std::numeric_limits<double>().max();
		long found_e = -1;

		Volume::Cell cell(_m, c);
		int nhalfedges = cell.nhalfedges(); 
		
		for (auto &f : cell.iter_facets()) {
			for (auto &h : f.iter_halfedges()) {
				
				long e = c * nhalfedges + h;

				// Get points from current edge
				vec3 p1 = h.from();
				vec3 p2 = h.to();
				vec3 b = (p1 + p2) * .5;
				// Compute dist from picked point to bary of edge points
				double d = (vec3(p0.x, p0.y, p0.z) - b).norm(); // TODO maybe use norm2 will give the same result

				// Keep min dist
				if (d < min_d) {
					min_d = d;
					found_e = e;
				}
			}
		}

		return found_e;
	}
	
	protected:

	std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const override {
		std::vector<std::pair<ElementKind, NamedContainer>> containers;
		
		for (auto &c : _volumeAttributes.points)
			containers.push_back({ElementKind::POINTS_ELT, c});
		for (auto &c : _volumeAttributes.cell_corners)
			containers.push_back({ElementKind::CELL_CORNERS_ELT, c});
		for (auto &c : _volumeAttributes.cell_facets)
			containers.push_back({ElementKind::CELL_FACETS_ELT, c});
		for (auto &c : _volumeAttributes.cells)
			containers.push_back({ElementKind::CELLS_ELT, c});

		return containers;
	}

	private:

	// Mesh
	TVolume _m;
	VolumeAttributes _volumeAttributes;
	
};

typedef VolumeModel<Tetrahedra> TetModel;
typedef VolumeModel<Hexahedra> HexModel;