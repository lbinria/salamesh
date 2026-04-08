#pragma once
#include "shader_params.h"

struct MaterialInstance {


	bool getVisible() const { return visible; }


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

	template<typename T>
	std::shared_ptr<T> getParams(const std::string name) {
		if (!params.contains(name))
			return nullptr;

		return std::static_pointer_cast<T>(params.at(name));
	}

	// Shortcuts to some common parameters

	std::shared_ptr<LightParams> getLightParams() {
		return getParams<LightParams>("light");
	}

	std::shared_ptr<ClippingParams> getClippingParams() {
		return getParams<ClippingParams>("clipping");
	}

	std::shared_ptr<MeshParams> getMeshParams() {
		return getParams<MeshParams>("mesh");
	}

	std::shared_ptr<HalfedgeParams> getHalfedgeParams() {
		return getParams<HalfedgeParams>("halfedges");
	}

	std::shared_ptr<PointSetParams> getPointSetParams() {
		return getParams<PointSetParams>("points");
	}

	// TODO add saveState, loadState
	std::shared_ptr<ShaderParams> getParam(const std::string name) const {
		return params.at(name);
	}

	void addBufferGroup(const std::string name, std::shared_ptr<ShaderBufferGroup> bufferGroup) {
		buffers[name] = bufferGroup;
	}

	private:
	bool visible = true;
	std::map<std::string, std::shared_ptr<ShaderParams>> params;
	std::map<std::string, std::shared_ptr<ShaderBufferGroup>> buffers;

};