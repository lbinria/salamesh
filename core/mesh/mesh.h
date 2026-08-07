#pragma once

#include <ultimaille/all.h>
using namespace UM;

#include "attribute.h"
#include "mesh_primitive.h"
#include <optional>
#include <float.h>

#include "json.hpp"
using json = nlohmann::json;

#include "helpers.h"

struct Mesh {

	Mesh() {
		_index = maxIndex;
		++maxIndex;
	}

	// Remove copy constructors
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	// Allow moves
	Mesh(Mesh&&) = default;
	Mesh& operator=(Mesh&&) = default;

	int getIndex() const {
		return _index;
	}

	static inline int getMaxIndex() {
		return maxIndex;
	}

	static void clearIndex() {
		maxIndex = 0;
	}

	virtual bool save() {
		if (path.empty())
			return false;

		return saveAs(path);
	}

	virtual bool saveAs(const std::string filename) = 0;
	virtual bool load(const std::string filename) = 0;

	void saveState(json &j, const std::string filename);
	void loadState(json &j, const std::string filename);

	virtual std::tuple<vec3, vec3> bbox() = 0;

	vec3 getCenter() {
		auto [bmin, bmax] = bbox();
		return (bmin + bmax) / 2.f;
	}

	double getRadius() {
		auto [bmin, bmax] = bbox();		
		return (bmax - bmin).norm2() / 2.f;
	}

	void requestUpdate() {
		_dirty = true;
	}

	bool shouldUpdate() const {
		return _dirty;
	}

	void updateDone() const {
		_dirty = false;
	}

	virtual int nverts() const = 0; 
	virtual int nfacets() const = 0; 
	virtual int ncells() const = 0; 
	virtual int ncorners() const = 0; 
	virtual int nhalfedges() const = 0;
	virtual int cellSize() = 0;

	virtual int facetSize(int f) const = 0;

	virtual std::vector<PointVertex> getPointsStream() = 0;
	virtual std::vector<TriangleVertex> getTrianglesStream() = 0;
	virtual std::vector<EdgeVertex> getEdgesStream() = 0;

	// TODO set private (pass in constructor)
	std::string path = "";

	std::vector<Attribute> getAttributes() {
		
		std::vector<Attribute> attributes;
		for (auto &[kind, c] : getAttributeContainers()) {
			auto attrs = getAttributesFromContainer(kind, c);
			for (auto a : attrs) {
				attributes.push_back(a);
			}
		}
		return attributes;

	}

	std::optional<Attribute> getAttribute(const std::string fullname) {

		for (auto &attr : getAttributes()) {
			if (attr.getFullName() == fullname)
				return attr;
		}

		return std::nullopt;
	}

	bool hasAttribute(const std::string fullname) {
		for (auto &attr : getAttributes()) {
			if (attr.getFullName() == fullname)
				return true;
		}

		return false;
	}

	bool hasAttribute(Attribute a) {
		for (auto &attr : getAttributes()) {
			if (attr == a)
				return true;
		}

		return false;
	}

	virtual void clean() {}

	protected:

	virtual std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const = 0;

	std::vector<Attribute> getAttributesFromContainer(ElementKind kind, NamedContainer &container) {
		std::vector<Attribute> attributes;
		
		// Get the type of the container
		ElementType type = ElementType::DOUBLE_ELT; // Default type
		if (auto a = dynamic_cast<AttributeContainer<double>*>(container.ptr.get())) {
			type = ElementType::DOUBLE_ELT;
		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(container.ptr.get())) {
			type = ElementType::INT_ELT;
		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(container.ptr.get())) {
			type = ElementType::BOOL_ELT;
		} else if (auto a = dynamic_cast<AttributeContainer<vec2>*>(container.ptr.get())) {
			type = ElementType::VEC2_ELT;
			attributes.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, true, 0);
			attributes.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, true, 1);
		} else if (auto a = dynamic_cast<AttributeContainer<vec3>*>(container.ptr.get())) {
			type = ElementType::VEC3_ELT;
			attributes.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, true, 0);
			attributes.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, true, 1);
			attributes.emplace_back(container.name + "[2]", kind, ElementType::DOUBLE_ELT, container.ptr, true, 2);
		} else {
			throw std::runtime_error("Unknown attribute type for container: " + container.name);
		}

		attributes.emplace_back(container.name, kind, type, container.ptr, false, -1);

		return attributes;
	}

	private:
	mutable bool _dirty = true;

	static inline int maxIndex = 0;
	int _index;
};

// Define concept to accept only types that are derived from Surface
template<typename T>
concept SurfaceDerived = std::is_base_of_v<Surface, std::remove_cv_t<T>> && 
	!std::is_same_v<Surface, std::remove_cv_t<T>>;

// Define concept to accept only types that are derived from Volume
template<typename T>
concept VolumeDerived = std::is_base_of_v<Volume, std::remove_cv_t<T>> && 
	!std::is_same_v<Volume, std::remove_cv_t<T>>;

template<SurfaceDerived TSurface>
struct SurfaceMesh : public Mesh {

	bool saveAs(const std::string filename) override {
		// Check path validity
		if (filename.empty()) {
			std::cerr << "Error: No path specified for saving the mesh." << std::endl;
			return false;
		}
		
		// Save attributes ! Convert back from salamesh attributes to NamedContainer vectors
		std::vector<NamedContainer> point_attrs;
		std::vector<NamedContainer> facet_attrs;
		std::vector<NamedContainer> corner_attrs;
		for (auto &a : getAttributes()) {
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

		write_by_extension(filename, getSurface(), attributes);

		return true;
	}

	bool load(const std::string filename) {
		_attributes = read_by_extension(filename, _m);
		return true;
	}

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

	// Check whether all facets have the same size
	int cellSize() override {
		int s = -1;
		for (auto &f : _m.iter_facets()) {
			if (s > 0 && f.size() != s)
				return -1;

			s = f.size();
		}
		return s;
	}

	int facetSize(int f) const override {
		return _m.facet_size(f);
	}

	std::tuple<vec3, vec3> bbox() override {
		vec3 min = vec3(FLT_MAX);
		vec3 max = vec3(-FLT_MAX);

		for (auto &v : _m.iter_vertices()) {
			vec3 p = v;
			min = sl::min(min, p);
			max = sl::max(max, p);
		}

		return {min, max};
	}

	std::vector<PointVertex> getPointsStream() final override {
		std::vector<PointVertex> points(_m.nverts());
		for (auto &v : _m.iter_vertices()) {
			vec3 p = v;
			points[v] = { 
				.id = v, 
				.pos = sl::algebra::vecf(p), 
				.normal = {0,0,0},
				.size = 1.f
			};
		}
		return points;
	}

	std::vector<EdgeVertex> getEdgesStream() final override {
		std::vector<EdgeVertex> vertices;
		// pre-allocate to speed-up
		vertices.reserve(_m.nfacets() * 4 /* reserve for 4 side facets */ * 6 /* 1 quad, 2 tri per quad, 3 points per tri */); 

		for (auto &f : _m.iter_facets()) {
			int facetSize = _m.facet_size(f);

			vec3 b;
			for (int lv = 0; lv < f.size(); ++lv) {
				b += f.vertex(lv).pos();
			}
			b /= f.size();
			sl::algebra::vec3 bary = sl::algebra::vecf(b);

			for (int i = 0; i < facetSize; ++i) {
				
				auto v0 = f.vertex(i);
				auto v1 = f.vertex((i + 1) % facetSize);
				auto p0 = v0.pos();
				auto p1 = v1.pos();
				
				int halfedgeIdx = f * facetSize + i;

				sl::algebra::vec3 gp0 = sl::algebra::vecf(p0);
				sl::algebra::vec3 gp1 = sl::algebra::vecf(p1);

				// build the 4 “corner” vertices
				EdgeVertex lv0{halfedgeIdx, gp0, gp1, -1.0f, 0.0f, bary};  // corner: start, left side
				EdgeVertex lv1{halfedgeIdx, gp0, gp1, +1.0f, 0.0f, bary};  // corner: start, right side
				EdgeVertex lv2{halfedgeIdx, gp0, gp1, -1.0f, 1.0f, bary};  // corner: end,   left side
				EdgeVertex lv3{halfedgeIdx, gp0, gp1, +1.0f, 1.0f, bary};  // corner: end,   right side

				vertices.push_back(lv0);
				vertices.push_back(lv1);
				vertices.push_back(lv2);
				
				vertices.push_back(lv2);
				vertices.push_back(lv3);
				vertices.push_back(lv1);

			}
		}

		return vertices;
	}

	std::vector<TriangleVertex> getTrianglesStream() override {

		int cornerOff = 0;
		std::vector<TriangleVertex> vertices;
		for (auto &f : _m.iter_facets()) {

			// There is as much triangles as vertices in facet,
			int nv = f.size();
			const int ntri = nv;

			// Compute bary
			vec3 bary{0.};
			for (int v = 0; v < nv; ++v) {
				auto pos = f.vertex(v).pos();
				bary += pos;
			}
			bary /= nv;

			// Compute normal
			std::vector<vec3> pts(nv);
			for (int v = 0; v < nv; ++v) {
				pts[v] = f.vertex(v).pos();
			}
			vec3 n = geo::normal(pts.data(), nv);

			

			for (int t = 0; t < ntri; ++t) {

				const int lv = t;
				// Three points of current triangle
				vec3 verts[3] = {bary , f.vertex(lv).pos(), f.vertex((lv + 1) % nv).pos()};

				// Compute first corner index of the triangle
				int firstCornerIdx = cornerOff + lv;

				for (int i = 0; i < 3; ++i) {

					// Retrieve vertex id (0 is always the barycenter => no vertex associated)
					int v = i == 0 ? -1 : f.vertex((lv + (i - 1)) % nv);

					auto p = verts[i];
					auto p1 = verts[1];
					auto p2 = verts[2];

					vertices.push_back({
						.id = v, // useless i think
						.facetIndex = f,
						.cellIndex = -1, // Don't care
						.localIndex = i,
						// .cornerIndex = firstCornerIdx,
						.cornerIndex = lv,
						.p = sl::algebra::vecf(p),
						.p0 = sl::algebra::vecf(bary),
						.p1 = sl::algebra::vecf(p1),
						.p2 = sl::algebra::vecf(p2),
						.normal = sl::algebra::vecf(n),
						.cornerOff = cornerOff
					});
				}
			}

			cornerOff += f.size();
		}

		return vertices;
	}

	std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const override {
		std::vector<std::pair<ElementKind, NamedContainer>> containers;
		
		for (auto &c : _attributes.points)
			containers.push_back({ElementKind::POINTS_ELT, c});
		for (auto &c : _attributes.corners) {
			containers.push_back({ElementKind::CORNERS_ELT, c});
			// containers.push_back({ElementKind::EDGES_ELT, c}); // TODO see pertinence
		}
		for (auto &c : _attributes.facets)
			containers.push_back({ElementKind::FACETS_ELT, c});

		return containers;
	}

	Surface& getSurface() { return _m; }
	const Surface& getSurface() const { return _m; }
	TSurface& getMesh() { return _m; }

	SurfaceAttributes _attributes;
	TSurface _m;

};

// typedef SurfaceMesh<Triangles> TrianglesMesh;
typedef SurfaceMesh<Quads> QuadsMesh;
typedef SurfaceMesh<Polygons> PolygonsMesh;

struct TrianglesMesh : public SurfaceMesh<Triangles> {


	std::vector<TriangleVertex> getTrianglesStream() final override {
		std::vector<TriangleVertex> vertices(_m.nfacets() * 3 /* 3 points per tri */);

		for (auto &f : _m.iter_facets()) {

			auto p0 = f.vertex(0).pos();
			auto p1 = f.vertex(1).pos();
			auto p2 = f.vertex(2).pos();

			for (int lv = 0; lv < 3; ++lv) {
				auto v = f.vertex(lv);
				auto p = v.pos();
				const int firstCornerIdx = f * 3;
				const int c = firstCornerIdx + lv;
				
				vertices[c] = { 
					.id = v,
					.facetIndex = f,
					.cellIndex = -1,
					.localIndex = lv,
					.cornerIndex = firstCornerIdx,
					.p = sl::algebra::vecf(p),
					.p0 = sl::algebra::vecf(p0),
					.p1 = sl::algebra::vecf(p1),
					.p2 = sl::algebra::vecf(p2)
				};
			}
		}
		return vertices;
	}

};

template<VolumeDerived TVolume>
struct VolumeMesh : public Mesh {

	bool saveAs(const std::string filename) override {
		// TODO important implement
		return true;
	}

	bool load(const std::string filename) {
		_attributes = read_by_extension(filename, _m);
		return true;
	}

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
		return _m.ncorners();
	}

	// Check whether all cells have the same size
	int cellSize() override {
		int s = -1;
		for (auto &c : _m.iter_cells()) {
			if (s > 0 && c.nfacets() != s)
				return -1;

			s = c.nfacets();
		}
		return s;
	}

	int facetSize(int f) const override {
		return _m.facet_size(f);
	}

	std::tuple<vec3, vec3> bbox() override {
		vec3 min = vec3(FLT_MAX);
		vec3 max = vec3(-FLT_MAX);

		for (auto &v : _m.iter_vertices()) {
			vec3 p = v;
			min = sl::min(min, p);
			max = sl::max(max, p);
		}

		return {min, max};
	}

	std::vector<PointVertex> getPointsStream() final override {
		std::vector<PointVertex> points(_m.nverts());
		for (auto &v : _m.iter_vertices()) {
			vec3 p = v;
			points[v] = { 
				.id = v, 
				.pos = sl::algebra::vecf(p), 
				.normal = {0,0,0},
				.size = 1.f
			};
		}
		return points;
	}

	std::vector<EdgeVertex> getEdgesStream() final override {
		// TODO implement
		return {};
	}

	std::vector<TriangleVertex> getTrianglesStream() final override {

		std::vector<TriangleVertex> vertices;
		vertices.reserve(_m.ncells() * 4 /* 4 facets per cell */ * 3 /* 3 points per facet */);

		for (auto &c : _m.iter_cells()) {

			vec3 v0 = c.vertex(0);
			vec3 v1 = c.vertex(1);
			vec3 v2 = c.vertex(2);
			vec3 v3 = c.vertex(3);

			vec3 b = (v0 + v1 + v2 + v3) / 4.;

			for (auto &f : c.iter_facets()) {
				
				vec3 p0 = f.vertex(0);
				vec3 p1 = f.vertex(1);
				vec3 p2 = f.vertex(2);

				for (int lv = 0; lv < 3; ++lv) {
					
					vertices.push_back({
						.id = f.vertex(lv),
						.facetIndex = f,
						.cellIndex = c,
						.localIndex = lv,
						.cornerIndex = -1, // Don't care
						.p = {0, 0, 0}, // Don't care
						.p0 = sl::algebra::vecf(p0),
						.p1 = sl::algebra::vecf(p1),
						.p2 = sl::algebra::vecf(p2),
						.bary = sl::algebra::vecf(b),
						.normal = {0, 0, 0} // Don't care
					});
				}
			}
		}

		return vertices;
	}

	std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const override {
		std::vector<std::pair<ElementKind, NamedContainer>> containers;
		
		for (auto &c : _attributes.points)
			containers.push_back({ElementKind::POINTS_ELT, c});
		// for (auto &c : _attributes.corners) {
		// 	containers.push_back({ElementKind::CORNERS_ELT, c});
		// 	// containers.push_back({ElementKind::EDGES_ELT, c}); // TODO see pertinence
		// }
		for (auto &c : _attributes.cell_facets)
			containers.push_back({ElementKind::CELL_FACETS_ELT, c});
		for (auto &c : _attributes.cells)
			containers.push_back({ElementKind::CELLS_ELT, c});

		return containers;
	}

	Volume& getVolume() { return _m; }
	const Volume& getVolume() const { return _m; }
	TVolume& getMesh() { return _m; }

	VolumeAttributes _attributes;
	TVolume _m;

};

typedef VolumeMesh<Tetrahedra> TetrahedrasMesh;


struct PolyLineMesh : public Mesh {

	// Remove copy constructors, allow moves
	PolyLineMesh() = default;
	PolyLineMesh(const PolyLineMesh&) = delete;
	PolyLineMesh(PolyLineMesh&&) = default;
	PolyLineMesh& operator=(const PolyLineMesh&) = delete;
	PolyLineMesh& operator=(PolyLineMesh&&) = default;

	bool saveAs(const std::string path) override {
		// Check path validity
		if (path.empty()) {
			std::cerr << "Error: No path specified for saving the mesh." << std::endl;
			return false;
		}
		
		// Save attributes ! Convert back from salamesh attributes to NamedContainer vectors
		std::vector<NamedContainer> point_attrs;
		std::vector<NamedContainer> edge_attrs;
		for (auto &a : getAttributes()) {
			// Do not save splitted attributes 
			// For example, attr : vec2 => attr[0], attr[1] aren't saved
			if (a.isSplit)
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

	bool load(const std::string filename) {
		_attributes = read_by_extension(filename, _m);
		return true;
	}

	std::tuple<vec3, vec3> bbox() override {
		vec3 min = vec3(FLT_MAX);
		vec3 max = vec3(-FLT_MAX);

		for (auto &v : _m.iter_vertices()) {
			vec3 p = v;
			min = sl::min(min, p);
			max = sl::max(max, p);
		}

		return {min, max};
	}

	int nverts() const override {
		return _m.nverts();
	} 
	
	int nfacets() const override {
		return 0;
	}

	int ncells() const override {
		return 0;
	}

	int ncorners() const override {
		return 0;
	}

	int nhalfedges() const override {
		return _m.nedges();
	}

	int cellSize() override {
		return 2;
	}

	int facetSize(int f) const override {
		return 0;
	}

	std::vector<PointVertex> getPointsStream() final override {
		std::vector<PointVertex> points(_m.nverts());
		for (auto &v : _m.iter_vertices()) {
			vec3 p = v;
			points[v] = { 
				.id = v, 
				.pos = sl::algebra::vecf(p), 
				.normal = {0,0,0},
				.size = 1.f
			};
		}
		return points;
	}

	std::vector<EdgeVertex> getEdgesStream() final override {
		std::vector<EdgeVertex> vertices;
		// pre-allocate to speed-up
		vertices.reserve(_m.nedges() * 6 /* 1 quad, 2 tri per quad, 3 points per tri */); 

		for (auto &e : _m.iter_edges()) {

			vec3 b = vec3(e.from() + e.to()) * .5;
			sl::algebra::vec3 bary = sl::algebra::vecf(b);

			sl::algebra::vec3 gp0 = sl::algebra::vecf(e.from());
			sl::algebra::vec3 gp1 = sl::algebra::vecf(e.to());

			// build the 4 “corner” vertices
			EdgeVertex lv0{e, gp0, gp1, -1.0f, 0.0f, bary};  // corner: start, left side
			EdgeVertex lv1{e, gp0, gp1, +1.0f, 0.0f, bary};  // corner: start, right side
			EdgeVertex lv2{e, gp0, gp1, -1.0f, 1.0f, bary};  // corner: end,   left side
			EdgeVertex lv3{e, gp0, gp1, +1.0f, 1.0f, bary};  // corner: end,   right side

			vertices.push_back(lv0);
			vertices.push_back(lv1);
			vertices.push_back(lv2);
			
			vertices.push_back(lv2);
			vertices.push_back(lv3);
			vertices.push_back(lv1);

		}

		return vertices;
	}

	std::vector<TriangleVertex> getTrianglesStream() final override {
		return {};
	}

	PolyLineAttributes _attributes;
	PolyLine _m;

	protected:
	std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const override {
		std::vector<std::pair<ElementKind, NamedContainer>> containers;
		
		for (auto &c : _attributes.points)
			containers.push_back({ElementKind::POINTS_ELT, c});
		for (auto &c : _attributes.edges) {
			containers.push_back({ElementKind::EDGES_ELT, c});
		}
		return containers;
	}


};