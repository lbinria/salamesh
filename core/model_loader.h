#pragma once 
#include "scene_node.h"

#include <ultimaille/all.h>
using namespace UM;

#include <string>

struct Scene;

struct ModelLoader {

	ModelLoader(Scene &scene) : _scene(scene) {}

	// // Remove copy constructors, allow moves
	// ModelLoader(const ModelLoader&) = delete;
	// ModelLoader& operator=(const ModelLoader&) = delete;
	// ModelLoader(ModelLoader&&) = default;
	// ModelLoader& operator=(ModelLoader&&) = default;

	bool loadTriangles(const std::string &filename, SceneNode &node) {
		auto geometry = std::make_unique<TrianglesGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		geometry->path = filename;
		node.setGeometry(std::move(geometry));

		return true;
	}

	bool loadQuads(const std::string &filename, SceneNode &node) {
		auto geometry = std::make_unique<QuadsGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		geometry->path = filename;
		node.setGeometry(std::move(geometry));

		return true;
	}

	bool loadPolygons(const std::string &filename, SceneNode &node) {
		auto geometry = std::make_unique<PolygonsGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nfacets() <= 0)
			return false;

		geometry->path = filename;
		node.setGeometry(std::move(geometry));

		return true;
	}

	bool loadPolyLine(const std::string &filename, SceneNode &node) {
		auto geometry = std::make_unique<PolyLineGeometry>();

		geometry->_attributes = read_by_extension(filename, geometry->_m);
		if (geometry->_m.nedges() <= 0)
			return false;

		geometry->path = filename;
		node.setGeometry(std::move(geometry));

		return true;
	}

	std::shared_ptr<SceneNode> load(const std::string filename);

	private:
	Scene &_scene;

};