#pragma once
#include "mesh.h"

struct LinesMesh : public Mesh {

	struct Line {
		vec3 a;
		vec3 b;
		vec3 color;
	};

	bool save() override {
		throw std::runtime_error("`save` is not implemented on `LineMesh`");
	}

	bool saveAs(const std::string filename) override {
		throw std::runtime_error("`saveAs` is not implemented on `LineMesh`");
	}

	bool load(const std::string filename) override {
		throw std::runtime_error("`load` is not implemented on `LineMesh`");
	}

	int nverts() const override {
		return _lines.size() * 2;
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
		return _lines.size();
	}

	long pickEdge(vec3 p0, int f) override {
		return -1;
	}

	std::tuple<vec3, vec3> bbox() override {
		vec3 min(FLT_MAX);
		vec3 max(-FLT_MAX);

		for (auto &l : _lines) {
			min = sl::min(sl::min(min, l.a), l.b);
			max = sl::max(sl::max(max, l.a), l.b);
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