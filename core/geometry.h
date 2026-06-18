#pragma once

#include <ultimaille/all.h>
using namespace UM;

#include "data/attribute.h"
#include <optional>

struct Geometry {

	virtual bool save() = 0;
	virtual bool saveAs(const std::string filename) = 0;

	virtual std::tuple<glm::vec3, glm::vec3> bbox() = 0;

	glm::vec3 getCenter() {
		auto [bmin, bmax] = bbox();
		return (bmin + bmax) / 2.f;
	}

	double getRadius() {
		auto [bmin, bmax] = bbox();
		return glm::length(bmax - bmin) / 2.f;
	}

	void requestUpdate() {
		_dirty = true;
	}

	bool shouldUpdate() const {
		return _dirty;
	}

	void updateDone() {
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

	virtual long pickEdge(glm::vec3 p0, int c) = 0;

	private:
	bool _dirty = true;
};

struct MeshGeometry : public Geometry {

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
			auto attrs = getAttibutesFromContainer(kind, c);
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

	std::string path = "";

	protected:

	virtual std::vector<std::pair<ElementKind, NamedContainer>> getAttributeContainers() const = 0;

	std::vector<Attribute> getAttibutesFromContainer(ElementKind kind, NamedContainer &container) {
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
struct SurfaceGeometry : public MeshGeometry {

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

	std::tuple<glm::vec3, glm::vec3> bbox() override {
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);

		for (auto &v : _m.iter_vertices()) {
			glm::vec3 p = sl::um2glm(v);
			min = glm::min(min, p);
			max = glm::max(max, p);
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

typedef SurfaceGeometry<Triangles> TrianglesGeometry;
typedef SurfaceGeometry<Quads> QuadsGeometry;
typedef SurfaceGeometry<Polygons> PolygonsGeometry;


struct PolyLineGeometry : public MeshGeometry {

	// Remove copy constructors, allow moves
	PolyLineGeometry() = default;
	PolyLineGeometry(const PolyLineGeometry&) = delete;
	PolyLineGeometry(PolyLineGeometry&&) = default;
	PolyLineGeometry& operator=(const PolyLineGeometry&) = delete;
	PolyLineGeometry& operator=(PolyLineGeometry&&) = default;

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

	std::tuple<glm::vec3, glm::vec3> bbox() override {
		glm::vec3 min = glm::vec3(FLT_MAX);
		glm::vec3 max = glm::vec3(-FLT_MAX);

		for (auto &v : _m.iter_vertices()) {
			glm::vec3 p = sl::um2glm(v);
			min = glm::min(min, p);
			max = glm::max(max, p);
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

	long pickEdge(glm::vec3 p0, int f) override {
		return 0;
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

struct LinesGeometry : public Geometry {

	struct Line {
		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 color;
	};

	bool save() override {
		throw std::runtime_error("`save` is not implemented on `LineGeometry`");
	}

	bool saveAs(const std::string filename) override {
		throw std::runtime_error("`saveAs` is not implemented on `LineGeometry`");
	}

	// Remove copy constructors, allow moves
	LinesGeometry() = default;
	LinesGeometry(const LinesGeometry&) = delete;
	LinesGeometry(LinesGeometry&&) = default;
	LinesGeometry& operator=(const LinesGeometry&) = delete;
	LinesGeometry& operator=(LinesGeometry&&) = default;

	std::tuple<glm::vec3, glm::vec3> bbox() override {
		glm::vec3 min(FLT_MAX);
		glm::vec3 max(-FLT_MAX);

		for (auto &l : _lines) {
			min = glm::min(glm::min(min, l.a), l.b);
			max = glm::max(glm::max(max, l.a), l.b);
		}

		return {min, max};
	}

	void clearLines() {
		_lines.clear();
	}

	// TODO generate guid for line
	void addLine(Line line) {
		_lines.push_back(line);
	}

	void addLines(std::vector<Line> allLines) {
		_lines.insert(_lines.end(), allLines.begin(), allLines.end());
	}

	const std::vector<Line> getLines() const {
		return _lines;
	}

	private:

	std::vector<Line> _lines;

};

// For volume
// long pickEdge(glm::vec3 p0, int c) override {
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