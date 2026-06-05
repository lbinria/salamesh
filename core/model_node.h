#pragma once
#include "scene_node.h"
struct Scene;

struct ModelNode : SceneNode {

	ModelNode(Scene &scene) : 
		_scene(scene)
	{}

	void setSelectedColormap(int idx);
	int getSelectedColormap();

	private:
	Scene &_scene;

	std::string _selectedAttribute;
	std::map<std::tuple<Layer, ElementKind>, std::string> _attrNameByLayerAndKind;

};