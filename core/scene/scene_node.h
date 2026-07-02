#pragma once
#include "helpers.h"
#include "shader.h"
#include "geometry.h"
#include "geometry_buffer.h"
#include "shader_base.h"
#include "colormap.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct SceneNode : std::enable_shared_from_this<SceneNode> {

	SceneNode(std::string name, std::shared_ptr<Geometry> geometry) :  
	_name(name),
	_geometry(geometry) {

	}

	// Remove copy
	SceneNode(const SceneNode&) = delete;
	SceneNode& operator=(const SceneNode&) = delete;
	// Allow move
	SceneNode(SceneNode&&) = default;
	SceneNode& operator=(SceneNode&&) = default;

	void loadState(json &j, const std::string filename);
	void saveState(json &j, const std::string filename);



	Geometry& getGeometry() {
		return *_geometry;
	}

	// TODO remove! 
	void requestUpdate() {
		_geometry->requestUpdate();
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

	std::optional<std::reference_wrapper<GeometryBuffer>> getGeometryBuffer(const std::string name) {
		if (!_geometryBuffer.contains(name))
			return std::nullopt;
		
		return _geometryBuffer.at(name);
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
	void updateLayers();
	
	// void updateLayer(Layer layer, ElementKind kind);
	// void resetLayer(ElementKind kind, Layer layer);
	
	vec3 position{0,0,0};

	private:
	std::string _name;

	std::shared_ptr<Geometry> _geometry;

	std::map<std::string, GeometryBuffer> _geometryBuffer;
	std::map<std::string, Material> _materials;


	bool _visible = true;

	std::string _selectedAttribute;
	std::map<std::tuple<Layer, ElementKind>, std::string> _attrNameByLayerAndKind;

};