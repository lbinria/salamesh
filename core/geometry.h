#pragma once

#include <ultimaille/all.h>
using namespace UM;


struct Geometry {

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

	private:
	bool _dirty = true;
};

struct MeshGeometry : public Geometry {
	virtual int nverts() const = 0; 
	virtual int nfacets() const = 0; 
	virtual int ncells() const = 0; 
	virtual int ncorners() const = 0; 
	virtual int nhalfedges() const = 0;


	std::vector<Attribute> getAttributes() {
		
		std::vector<Attribute> attributes;
		for (auto &[kind, c] : getAttributeContainers()) {
			auto attrs = getAttibutesFromContainer(kind, c);
			for (auto a : attrs) {
				attributes.push_back(a);
			}
		}
		return attributes;

	}


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
			attributes.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
			attributes.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
		} else if (auto a = dynamic_cast<AttributeContainer<vec3>*>(container.ptr.get())) {
			type = ElementType::VEC3_ELT;
			attributes.emplace_back(container.name + "[0]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
			attributes.emplace_back(container.name + "[1]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
			attributes.emplace_back(container.name + "[2]", kind, ElementType::DOUBLE_ELT, container.ptr, true);
		} else {
			throw std::runtime_error("Unknown attribute type for container: " + container.name);
		}

		attributes.emplace_back(container.name, kind, type, container.ptr, false);

		return attributes;
	}

};

struct TrianglesGeometry : public Geometry {

	// Remove copy constructors, allow moves
	TrianglesGeometry() = default;
	TrianglesGeometry(const TrianglesGeometry&) = delete;
	TrianglesGeometry(TrianglesGeometry&&) = default;
	TrianglesGeometry& operator=(const TrianglesGeometry&) = delete;
	TrianglesGeometry& operator=(TrianglesGeometry&&) = default;

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

	SurfaceAttributes _attributes;
	Triangles _m;
};

struct QuadsGeometry : public Geometry {

	// Remove copy constructors, allow moves
	QuadsGeometry() = default;
	QuadsGeometry(const QuadsGeometry&) = delete;
	QuadsGeometry(QuadsGeometry&&) = default;
	QuadsGeometry& operator=(const QuadsGeometry&) = delete;
	QuadsGeometry& operator=(QuadsGeometry&&) = default;

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
	
	SurfaceAttributes _attributes;
	Quads _m;
};

struct PolygonsGeometry : public Geometry {

	// Remove copy constructors, allow moves
	PolygonsGeometry() = default;
	PolygonsGeometry(const PolygonsGeometry&) = delete;
	PolygonsGeometry(PolygonsGeometry&&) = default;
	PolygonsGeometry& operator=(const PolygonsGeometry&) = delete;
	PolygonsGeometry& operator=(PolygonsGeometry&&) = default;

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

	SurfaceAttributes _attributes;
	Polygons _m;
};

struct PolyLineGeometry : public MeshGeometry {

	// Remove copy constructors, allow moves
	PolyLineGeometry() = default;
	PolyLineGeometry(const PolyLineGeometry&) = delete;
	PolyLineGeometry(PolyLineGeometry&&) = default;
	PolyLineGeometry& operator=(const PolyLineGeometry&) = delete;
	PolyLineGeometry& operator=(PolyLineGeometry&&) = default;

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