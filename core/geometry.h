#pragma once

#include <ultimaille/all.h>
using namespace UM;


struct Geometry {

	virtual std::tuple<glm::vec3, glm::vec3> bbox() = 0;

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

	Quads _m;
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