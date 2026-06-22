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
		auto &geometry = node.createGeometry<TrianglesGeometry>();

		geometry._attributes = read_by_extension(filename, geometry._m);
		if (geometry._m.nfacets() <= 0)
			return false;

		geometry.path = filename;

		return true;
	}

	bool loadQuads(const std::string &filename, SceneNode &node) {
		auto &geometry = node.createGeometry<QuadsGeometry>();

		geometry._attributes = read_by_extension(filename, geometry._m);
		if (geometry._m.nfacets() <= 0)
			return false;

		geometry.path = filename;

		return true;
	}

	bool loadPolygons(const std::string &filename, SceneNode &node) {
		auto &geometry = node.createGeometry<PolygonsGeometry>();

		geometry._attributes = read_by_extension(filename, geometry._m);
		if (geometry._m.nfacets() <= 0)
			return false;

		geometry.path = filename;

		return true;
	}

	bool loadPolyLine(const std::string &filename, SceneNode &node) {
		auto &geometry = node.createGeometry<PolyLineGeometry>();

		geometry._attributes = read_by_extension(filename, geometry._m);
		if (geometry._m.nedges() <= 0)
			return false;

		geometry.path = filename;

		return true;
	}

	std::shared_ptr<SceneNode> load(const std::string filename, const std::string name);

	private:
	Scene &_scene;

};