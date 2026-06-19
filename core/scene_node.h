#pragma once
#include "helpers.h"
#include "shader.h"
#include "geometry/geometry.h"
#include "shader_buffer.h"
#include "shaders/shader_base.h"
#include "data/colormap.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct SceneNode : std::enable_shared_from_this<SceneNode> {


	SceneNode() :  
	_name(sl::generateGuid()) {
		_index = maxIndex;
		++maxIndex;
	}

	SceneNode(std::string name) :  
	_name(name) {
		_index = maxIndex;
		++maxIndex;
	}

	int getIndex() {
		return _index;
	}

	static inline int getMaxIndex() {
		return maxIndex;
	}

	static void clearIndex() {
		maxIndex = 0;
	}

	std::tuple<glm::vec3, glm::vec3> bbox() {
		auto [min, max] = _geometry->bbox();

		for (auto child : _children) {
			auto [childMin, childMax] = child->bbox();
			min = glm::min(min, childMin);
			max = glm::max(max, childMax);
		}

		return {min, max};
	}

	Geometry& getGeometry() {
		return *_geometry;
	}

	void setGeometry(std::unique_ptr<Geometry> geometry) {
		_geometry = std::move(geometry);
	}

	template<typename TGeometry>
	TGeometry& createGeometry() {
		_geometry = std::make_unique<TGeometry>();
		return *static_cast<TGeometry*>(_geometry.get());
	}

	bool addShader(ShaderBase &shader) {
		if (_geometryBuffer.contains(shader.getName()))
			return false;
		
		auto shaderBuffer = shader.createShaderBuffer();
		_geometryBuffer.emplace(shader.getName(), std::move(shaderBuffer));
		
		auto material = shader.createMaterial();
		_materials.emplace(shader.getName(), std::move(material));

		return true;
	}

	bool hasMaterial(const std::string name) {
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

	std::map<std::string, ShaderBuffer>& getShaderBuffers() {
		return _geometryBuffer;
	}

	std::optional<std::reference_wrapper<ShaderBuffer>> getShaderBuffer(ShaderBase &shader) {
		return getShaderBuffer(shader.getName());
	}

	std::optional<std::reference_wrapper<ShaderBuffer>> getShaderBuffer(const std::string name) {
		if (!_geometryBuffer.contains(name))
			return std::nullopt;
		
		return _geometryBuffer.at(name);
	}

	std::shared_ptr<SceneNode> getParent() const {
		return _parent.lock();
	}

	void add(std::shared_ptr<SceneNode> child) {
		child->_parent = shared_from_this();
		_children.push_back(child);
	}

	glm::vec3 getWorldPosition() const {
		if (auto p = _parent.lock()) {
			return p->getWorldPosition() + position;
		} else {
			return position;
		}
	}

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
	// void updateLayer(Layer layer, ElementKind kind);
	// void resetLayer(ElementKind kind, Layer layer);
	


	glm::vec3 position{0,0,0};

	private:
	std::string _name;

	std::unique_ptr<Geometry> _geometry;
	std::map<std::string, ShaderBuffer> _geometryBuffer;
	std::map<std::string, Material> _materials;

	std::weak_ptr<SceneNode> _parent;
	std::vector<std::shared_ptr<SceneNode>> _children;

	static inline int maxIndex = 0;
	int _index;

	bool _visible = true;

	std::string _selectedAttribute;
	std::map<std::tuple<Layer, ElementKind>, std::string> _attrNameByLayerAndKind;

};