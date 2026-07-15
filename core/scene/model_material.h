#pragma once
#include "scene_node.h"

struct ModelMaterial {

	ModelMaterial(SceneNode &node) : _node(node) {}

	bool getLight();
	void setLight(bool enabled);

	private:
	SceneNode &_node;
};