#pragma once
#include "shader_params.h"

struct MaterialInstance {


	bool getVisible() const { 
		return visible; 
	}
	
	void setVisible(bool val) { 
		visible = val; 
	}

	void apply(Shader &shader) const {
		if (!visible)
			return;
		
		for (auto &[k, p] : params) { 
			p->apply(shader);
		}

		for (auto &[k, b] : buffers) {
			b->apply(shader);
		}
	}

	void addParam(const std::string name, std::shared_ptr<ShaderParams> param) {
		params[name] = param;
	}

	bool hasParam(const std::string name) {
		return params.contains(name);
	}

	void addBufferGroup(const std::string name, std::shared_ptr<ShaderBufferGroup> bufferGroup) {
		buffers[name] = bufferGroup;
	}

	template<typename T>
	std::shared_ptr<T> getParams(const std::string name) const {
		if (!params.contains(name))
			return nullptr;

		return std::static_pointer_cast<T>(params.at(name));
	}

	template<typename T>
	std::shared_ptr<T> getBuffers(const std::string name) const {
		if (!buffers.contains(name))
			return nullptr;

		return std::static_pointer_cast<T>(buffers.at(name));
	}

	// Shortcuts to some common parameters

	std::shared_ptr<LightParams> getLightParams() const {
		return getParams<LightParams>("light");
	}

	std::shared_ptr<ClippingParams> getClippingParams() const {
		return getParams<ClippingParams>("clipping");
	}

	std::shared_ptr<MeshParams> getMeshParams() const {
		return getParams<MeshParams>("mesh");
	}

	std::shared_ptr<HalfedgeParams> getHalfedgeParams() const {
		return getParams<HalfedgeParams>("edges");
	}

	std::shared_ptr<PointSetParams> getPointSetParams() const {
		return getParams<PointSetParams>("points");
	}
	
	std::shared_ptr<LayerBufferGroup> getLayerBuffers() const {
		return getBuffers<LayerBufferGroup>("layers");
	}

	// TODO add saveState, loadState



	private:
	bool visible = true;
	std::map<std::string, std::shared_ptr<ShaderParams>> params;
	std::map<std::string, std::shared_ptr<ShaderBufferGroup>> buffers;

};

// Proxy
struct MaterialInstanceCollection {

	MaterialInstanceCollection(
		std::map<std::string, MaterialInstance>& materials, 
		std::map<std::string, std::string> rendererInfos) : 
		materials(materials), 
		rendererInfos(rendererInfos) {}

	MaterialInstance& getMaterial(const std::string rendererName) {
		auto rendererId = rendererInfos[rendererName];
		return materials.at(rendererId);
	}

	bool getVisible() {
		// At least one
		for (auto &[rn, rid] : rendererInfos) {
			if (materials[rid].getVisible())
				return true;
		}
		return false;
	}

	void setVisible(bool val) {
		for (auto &[rn, rid] : rendererInfos) {
			materials[rid].setVisible(val);
		}
	}

	private:
	std::map<std::string, MaterialInstance>& materials;
	std::map<std::string, std::string> rendererInfos;

};