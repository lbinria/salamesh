#pragma once
#include "helpers.h"
#include "shaders/shader.h"
#include "geometry.h"
#include "geometry_buffer.h"
#include "shaders/shader_base.h"
#include "colormap.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct SceneNode : std::enable_shared_from_this<SceneNode> {

	SceneNode(std::string name) :  
	_name(name) {
		_index = maxIndex;
		++maxIndex;
	}

	// Remove copy
	SceneNode(const SceneNode&) = delete;
	SceneNode& operator=(const SceneNode&) = delete;
	// Allow move
	SceneNode(SceneNode&&) = default;
	SceneNode& operator=(SceneNode&&) = default;

	int getIndex() const {
		return _index;
	}

	static inline int getMaxIndex() {
		return maxIndex;
	}

	static void clearIndex() {
		maxIndex = 0;
	}

	std::tuple<glm::vec3, glm::vec3> bbox() const;

	// TODO warning geometry can be null
	Geometry& getGeometry() {
		return *_geometry;
	}

	// // TODO warning geometry can be null
	// template<typename TGeometry>
	// TGeometry& getGeometry() {
	// 	return static_cast<TGeometry&>(*_geometry);
	// }

	template<typename TGeometry>
	TGeometry& createGeometry() {
		_geometry = std::make_unique<TGeometry>();
		return *static_cast<TGeometry*>(_geometry.get());
	}

	void requestUpdate() {
		if (_geometry)
			_geometry->requestUpdate();

		for (auto &c : _children)
			c->requestUpdate();
	}

	bool addShaderPass(ShaderBase &shader);

	bool hasMaterial(const std::string name) const {
		return _materials.contains(name);
	}

	std::map<std::string, Material>& getMaterials() {
		return _materials;
	}

	std::optional<std::reference_wrapper<Material>> getMaterial(const std::string name) {
		if (!_materials.contains(name))
			return std::nullopt;
		
		return _materials.at(name);
	}

	std::map<std::string, GeometryBuffer>& getGeometryBuffer() {
		return _geometryBuffer;
	}

	std::optional<std::reference_wrapper<GeometryBuffer>> getGeometryBuffer(ShaderBase &shader) {
		return getGeometryBuffer(shader.getName());
	}

	std::optional<std::reference_wrapper<GeometryBuffer>> getGeometryBuffer(const std::string name) {
		if (!_geometryBuffer.contains(name))
			return std::nullopt;
		
		return _geometryBuffer.at(name);
	}

	std::shared_ptr<SceneNode> getParent() const {
		return _parent.lock();
	}

	const std::vector<std::shared_ptr<SceneNode>>& getChildren() const {
		return _children;
	}

	void addChild(std::shared_ptr<SceneNode> child) {
		child->_parent = shared_from_this();
		_children.push_back(child);
	}

	std::vector<std::shared_ptr<SceneNode>> findChildrenRecursive();

	glm::vec3 getWorldPosition() const;

	const std::string getName() const {
		return _name;
	}

	void setVisible(bool visible) {
		_visible = visible;
	}

	bool isVisible() const {
		return _visible;
	}

	std::optional<Colormap> getColormap();
	void setColormap(Colormap colormap);


	std::string getLayerAttr(Layer layer, ElementKind kind);
	void setLayerAttr(Layer layer, ElementKind kind, const std::string name);

	void setLayer(Layer layer, ElementKind kind, bool update);
	void setLayer(Layer layer, ElementKind kind, const std::string attributeName, bool update);

	void unsetLayer(Layer layer, ElementKind kind, bool reset = false);
	void unsetLayers(bool reset = false);
	void updateLayers();
	
	// void updateLayer(Layer layer, ElementKind kind);
	// void resetLayer(ElementKind kind, Layer layer);
	
	glm::vec3 position{0,0,0};

	private:
	std::string _name;

	std::unique_ptr<Geometry> _geometry;
	std::map<std::string, GeometryBuffer> _geometryBuffer;
	std::map<std::string, Material> _materials;

	std::weak_ptr<SceneNode> _parent;
	std::vector<std::shared_ptr<SceneNode>> _children;

	static inline int maxIndex = 0;
	int _index;

	bool _visible = true;

	std::string _selectedAttribute;
	std::map<std::tuple<Layer, ElementKind>, std::string> _attrNameByLayerAndKind;

};