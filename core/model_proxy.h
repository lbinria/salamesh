#pragma once 
#include "data/layer.h"
struct SceneNode;
struct Scene;

#include <map>

// // Manage given node as an 3D Model entity
// struct ModelProxy {

// 	ModelProxy(Scene &scene, const std::weak_ptr<SceneNode>& node) : 
// 		_scene(scene), 
// 		_node(node)
// 	{}

// 	// Remove copy constructors, allow moves
// 	ModelProxy(const ModelProxy&) = delete;
// 	ModelProxy& operator=(const ModelProxy&) = delete;
// 	ModelProxy(ModelProxy&&) = default;
// 	ModelProxy& operator=(ModelProxy&&) = default;

// 	void setSelectedColormap(int idx);
// 	int getSelectedColormap();

// 	std::string getLayerAttr(Layer layer, ElementKind kind);
// 	void setLayerAttr(std::string name, Layer layer, ElementKind kind);

// 	void setLayer(Layer layer, ElementKind kind, bool update);
// 	void unsetLayer(ElementKind kind, Layer layer, bool reset);
// 	// void updateLayer(Layer layer, ElementKind kind);
// 	// void resetLayer(ElementKind kind, Layer layer);


// 	private:
// 	Scene &_scene;
// 	std::weak_ptr<SceneNode> _node;

// 	std::string _selectedAttribute;
// 	std::map<std::tuple<Layer, ElementKind>, std::string> _attrNameByLayerAndKind;
// 	// std::map<std::tuple<Layer, ElementKind>, bool> _activatedLayers;

// };