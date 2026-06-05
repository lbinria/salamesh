#pragma once 

#include <ultimaille/all.h>
using namespace UM;

#include "model_node.h"

#include <string>

struct ModelLoader {

	static bool loadTriangles(const std::string &filename, ModelNode &node) {
		auto geometry = std::make_unique<TrianglesGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		geometry->path = filename;
		node.setGeometry(std::move(geometry));

		return true;
	}

	static bool loadQuads(const std::string &filename, ModelNode &node) {
		auto geometry = std::make_unique<QuadsGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		geometry->path = filename;
		node.setGeometry(std::move(geometry));

		return true;
	}

	static bool loadPolygons(const std::string &filename, ModelNode &node) {
		auto geometry = std::make_unique<PolygonsGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		geometry->path = filename;
		node.setGeometry(std::move(geometry));

		return true;
	}

	static bool loadPolyLine(const std::string &filename, ModelNode &node) {
		auto geometry = std::make_unique<PolyLineGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nedges() <= 0)
			return false;

		geometry->path = filename;
		node.setGeometry(std::move(geometry));

		return true;
	}

	static ModelNode load(const std::string filename, Scene &scene) {
		ModelNode node(scene);

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