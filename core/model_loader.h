#pragma once 

#include <ultimaille/all.h>
using namespace UM;

#include "scene_node.h"

#include <string>

struct ModelLoader {

	static bool loadTriangles(const std::string &filename, SceneNode &node) {
		auto geometry = std::make_unique<TrianglesGeometry>();

		SurfaceAttributes attr = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		node.setGeometry(std::move(geometry));

		return true;
	}

	static bool loadQuads(const std::string &filename, SceneNode &node) {
		auto geometry = std::make_unique<QuadsGeometry>();

		SurfaceAttributes attr = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		node.setGeometry(std::move(geometry));

		return true;
	}

	static bool loadPolygons(const std::string &filename, SceneNode &node) {
		auto geometry = std::make_unique<PolygonsGeometry>();

		SurfaceAttributes attr = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		node.setGeometry(std::move(geometry));

		return true;
	}

	static bool loadPolyLine(const std::string &filename, SceneNode &node) {
		auto geometry = std::make_unique<PolyLineGeometry>();

		EdgeAttribute attr = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nedges() <= 0)
			return false;

		node.setGeometry(std::move(geometry));

		return true;
	}

	static SceneNode load(const std::string filename) {
		SceneNode node;

		bool success = loadTriangles(filename, node);
		
		if (!success)
			success = loadQuads(filename, node);

		if (!success)
			success = loadPolygons(filename, node);
		
		if (!success)
			success = loadPolyLine(filename, node);

		return node;
	}

};