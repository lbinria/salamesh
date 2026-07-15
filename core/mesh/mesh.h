#pragma once

#include <ultimaille/all.h>
using namespace UM;

#include "attribute.h"
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

	virtual bool save() = 0;
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

	virtual  std::vector<Attribute> getAttributes() {
		return {};
	}
	

	virtual std::optional<Attribute> getAttribute(const std::string name) {
		return std::nullopt;
	}

	virtual int nverts() const = 0; 
	virtual int nfacets() const = 0; 
	virtual int ncells() const = 0; 
	virtual int ncorners() const = 0; 
	virtual int nhalfedges() const = 0;
	virtual bool isRegular() = 0;

	virtual long pickEdge(vec3 p0, int c) = 0;

	// TODO set private (pass in constructor)
	std::string path = "";

	private:
	mutable bool _dirty = true;

	static inline int maxIndex = 0;
	int _index;
};

struct MeshMesh : public Mesh {

	// virtual int nverts() const = 0; 
	// virtual int nfacets() const = 0; 
	// virtual int ncells() const = 0; 
	// virtual int ncorners() const = 0; 
	// virtual int nhalfedges() const = 0;


	virtual bool save() {
		if (path.empty())
			return false;

		return saveAs(path);
	}

	std::vector<Attribute> getAttributes() override {
		
		std::vector<Attribute> attributes;
		for (auto &[kind, c] : getAttributeContainers()) {
			auto attrs = getAttributesFromContainer(kind, c);
			for (auto a : attrs) {
				attributes.push_back(a);
			}
		}
		return attributes;

	}

	std::optional<Attribute> getAttribute(const std::string name) override {
		std::string attrName = name;

		// Extract selectedDim from string
		int selectedDim = -1;
		auto lbrPos = name.find('[');
		auto rbrPos = name.find(']');
		// if "attr[0]" requested for example, we would like to see selectedDim=0, so nDims of attribute = 1
		if (lbrPos != std::string::npos && rbrPos != std::string::npos) {
			selectedDim = std::stoi(name.substr(lbrPos + 1, rbrPos - lbrPos));
			attrName = name.substr(0, lbrPos);
		}

		for (auto &[kind, c] : getAttributeContainers()) {
			if (c.name == attrName)
				return getAttributeFromContainer(kind, c, selectedDim);
		}

		return std::nullopt;
	}


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

	Attribute getAttributeFromContainer(ElementKind kind, NamedContainer &container, int selectedDim) {
		
		// Get the type of the container
		ElementType type = ElementType::DOUBLE_ELT; // Default type
		if (auto a = dynamic_cast<AttributeContainer<double>*>(container.ptr.get())) {
			type = ElementType::DOUBLE_ELT;
		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(container.ptr.get())) {
			type = ElementType::INT_ELT;
		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(container.ptr.get())) {
			type = ElementType::BOOL_ELT;
		} else if (auto a = dynamic_cast<AttributeContainer<vec2>*>(container.ptr.get())) {
			type = selectedDim < 0 ? ElementType::VEC2_ELT : ElementType::DOUBLE_ELT;
		} else if (auto a = dynamic_cast<AttributeContainer<vec3>*>(container.ptr.get())) {
			type = selectedDim < 0 ? ElementType::VEC3_ELT : ElementType::DOUBLE_ELT;
		} else {
			throw std::runtime_error("Unknown attribute type for container: " + container.name);
		}

		std::string attrName = container.name;
		if (selectedDim >= 0) {
			attrName += "[" + std::to_string(selectedDim) + "]";
		}

		return {attrName, kind, type, container.ptr, selectedDim >= 0, selectedDim};
	}

};

// Define concept to accept only types that are derived from Surface
template<typename T>
concept SurfaceDerived = std::is_base_of_v<Surface, std::remove_cv_t<T>> && 
	!std::is_same_v<Surface, std::remove_cv_t<T>>;

template<SurfaceDerived TSurface>
struct SurfaceMesh : public MeshMesh {

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

	// Check whehter all facets have the same size
	bool isRegular() override {
		int s = -1;
		for (auto &f : _m.iter_facets()) {
			if (s > 0 && f.size() != s)
				return false;

			s = f.size();
		}
		return true;
	}

	long pickEdge(vec3 p0, int f) override {
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
			double d = (p0 - b).norm(); // TODO maybe use norm2 will give the same result

			// Keep min dist
			if (d < min_d) {
				min_d = d;
				found_e = f * size + lv;
			}
		}

		return found_e;
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

typedef SurfaceMesh<Triangles> TrianglesMesh;
typedef SurfaceMesh<Quads> QuadsMesh;
typedef SurfaceMesh<Polygons> PolygonsMesh;


struct PolyLineMesh : public MeshMesh {

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

	bool isRegular() override {
		return true;
	}


	long pickEdge(vec3 p0, int f) override {
		return -1;
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



// For volume
// long pickEdge(vec3 p0, int c) override {
// 	// Search nearest edge
// 	double min_d = std::numeric_limits<double>().max();
// 	long found_e = -1;

// 	Volume::Cell cell(_m, c);
// 	int nhalfedges = cell.nhalfedges(); 
	
// 	for (auto &f : cell.iter_facets()) {
// 		for (auto &h : f.iter_halfedges()) {
			
// 			long e = c * nhalfedges + h;

// 			// Get points from current edge
// 			vec3 p1 = h.from();
// 			vec3 p2 = h.to();
// 			vec3 b = (p1 + p2) * .5;
// 			// Compute dist from picked point to bary of edge points
// 			double d = (vec3(p0.x, p0.y, p0.z) - b).norm(); // TODO maybe use norm2 will give the same result

// 			// Keep min dist
// 			if (d < min_d) {
// 				min_d = d;
// 				found_e = e;
// 			}
// 		}
// 	}

// 	return found_e;
// }