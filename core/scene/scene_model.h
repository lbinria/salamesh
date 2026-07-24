#pragma once
#include "helpers.h"
#include "shader.h"
#include "mesh.h"
#include "mesh_buffer.h"
#include "material.h"
#include "material_group.h"
#include "shader_base.h"
#include "colormap.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct SceneModel : std::enable_shared_from_this<SceneModel> {

	SceneModel(std::string name, std::shared_ptr<Mesh> mesh) :  
	_name(name),
	_mesh(mesh) {

	}

	// Remove copy
	SceneModel(const SceneModel&) = delete;
	SceneModel& operator=(const SceneModel&) = delete;
	// Allow move
	SceneModel(SceneModel&&) = default;
	SceneModel& operator=(SceneModel&&) = default;

	void loadState(json &j, const std::string filename);
	void saveState(json &j, const std::string filename);



	Mesh& getMesh() {
		return *_mesh;
	}

	// TODO remove! 
	void requestUpdate() {
		_mesh->requestUpdate();
	}

	bool addShaderPass(ShaderBase &shader);

	bool hasMaterial(const std::string name) const {
		return _materials.contains(name);
	}

	std::map<std::string, Material>& getMaterials() {
		return _materials;
	}

	MaterialParamsGroup getMaterialParamsGroup(const std::string paramName) {
		return MaterialParamsGroup(paramName, _materials);
	}

	std::optional<std::reference_wrapper<Material>> getMaterial(const std::string name) {
		if (!_materials.contains(name))
			return std::nullopt;
		
		return _materials.at(name);
	}

	std::map<std::string, MeshBuffer>& getMeshBuffer() {
		return _meshBuffer;
	}

	std::optional<std::reference_wrapper<MeshBuffer>> getMeshBuffer(const std::string name) {
		if (!_meshBuffer.contains(name))
			return std::nullopt;
		
		return _meshBuffer.at(name);
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
	
	vec3 position{0,0,0};

	private:
	std::string _name;

	std::shared_ptr<Mesh> _mesh;

	std::map<std::string, MeshBuffer> _meshBuffer;
	std::map<std::string, Material> _materials;


	bool _visible = true;

	std::string _selectedAttribute;
	std::map<std::tuple<Layer, ElementKind>, std::string> _attrNameByLayerAndKind;

};