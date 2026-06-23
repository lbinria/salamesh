#pragma once 
#include "../scene_node.h"

#include <ultimaille/all.h>
using namespace UM;

#include <string>

struct Scene;

struct ModelLoader {

	ModelLoader(Scene &scene) : _scene(scene) {}

	// Remove copy constructors, allow moves
	ModelLoader(const ModelLoader&) = delete;
	ModelLoader& operator=(const ModelLoader&) = delete;
	ModelLoader(ModelLoader&&) = default;
	ModelLoader& operator=(ModelLoader&&) = default;


	std::shared_ptr<SceneNode> load(const std::string filename, const std::string name);

	private:
	Scene &_scene;

	bool loadTriangles(const std::string &filename, SceneNode &node);
	bool loadQuads(const std::string &filename, SceneNode &node);
	bool loadPolygons(const std::string &filename, SceneNode &node);
	bool loadPolyLine(const std::string &filename, SceneNode &node);

};