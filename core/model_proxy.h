#pragma once 
#include "data/layer.h"
struct SceneNode;
struct Scene;

// Manage given node as an 3D Model entity
struct ModelProxy {

	ModelProxy(Scene &scene, const std::weak_ptr<SceneNode>& node) : 
		_scene(scene), 
		_node(node)
	{}

	// Remove copy constructors, allow moves
	ModelProxy(const ModelProxy&) = delete;
	ModelProxy& operator=(const ModelProxy&) = delete;
	ModelProxy(ModelProxy&&) = default;
	ModelProxy& operator=(ModelProxy&&) = default;

	void setSelectedColormap(int idx);
	int getSelectedColormap();

	private:
	Scene &_scene;
	std::weak_ptr<SceneNode> _node;

	std::string _selectedAttribute;

};