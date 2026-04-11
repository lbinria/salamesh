#pragma once
#include "shader_params.h"
#include <optional>

struct MaterialInstance {

	MaterialInstance() = default;
	MaterialInstance(const MaterialInstance&) = delete;
    MaterialInstance& operator=(const MaterialInstance&) = delete;

	MaterialInstance(MaterialInstance&&) noexcept = default;
    MaterialInstance& operator=(MaterialInstance&&) noexcept = default;

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

	// void addParamold(const std::string name, ShaderParams &param) {
	// 	params[name] = param;
	// }

	template<class TParam>
	void addParam(const std::string name) {
		params[name] = std::make_unique<TParam>();
	}

	bool hasParam(const std::string name) {
		return params.contains(name);
	}

	void addBufferGroup(const std::string name, std::shared_ptr<ShaderBufferGroup> bufferGroup) {
		buffers[name] = bufferGroup;
	}

	// template<typename T>
	// std::shared_ptr<T> getParams(const std::string name) const {
	// 	if (!params.contains(name))
	// 		return nullptr;

	// 	return std::static_pointer_cast<T>(params.at(name));
	// }

	template<typename T>
	std::shared_ptr<T> getBuffers(const std::string name) const {
		if (!buffers.contains(name))
			return nullptr;

		return std::static_pointer_cast<T>(buffers.at(name));
	}


	std::optional<std::reference_wrapper<ShaderParams>> getParams(const std::string name) {
		auto it = params.find(name);
		if (it == params.end()) return std::nullopt;
		return std::ref(*it->second);
	}

	// // Shortcuts to some common parameters
	// std::shared_ptr<LightParams> getLightParams() const {
	// 	return getParams<LightParams>("light");
	// }

	// std::shared_ptr<ClippingParams> getClippingParams() const {
	// 	return getParams<ClippingParams>("clipping");
	// }

	// std::shared_ptr<MeshParams> getMeshParams() const {
	// 	return getParams<MeshParams>("mesh");
	// }

	// std::shared_ptr<HalfedgeParams> getHalfedgeParams() const {
	// 	return getParams<HalfedgeParams>("edges");
	// }

	// std::shared_ptr<PointSetParams> getPointSetParams() const {
	// 	return getParams<PointSetParams>("points");
	// }
	
	std::shared_ptr<LayerBufferGroup> getLayerBuffers() const {
		return getBuffers<LayerBufferGroup>("layers");
	}

	// TODO add saveState, loadState



	private:
	bool visible = true;
	std::map<std::string, std::unique_ptr<ShaderParams>> params;
	std::map<std::string, std::shared_ptr<ShaderBufferGroup>> buffers;

};

// Proxy
struct MaterialInstanceCollection {

	MaterialInstanceCollection(
		std::map<std::string, MaterialInstance>& materials, 
		std::map<std::string, std::string> rendererInfos) : 
		materials(materials), 
		rendererInfos(rendererInfos) {}

	// MaterialInstance& getMaterial(const std::string rendererName) const {
	// 	auto rendererId = rendererInfos.at(rendererName);
	// 	return materials.at(rendererId);
	// }

	std::optional<std::reference_wrapper<MaterialInstance>> getMaterial(const std::string rendererName) const {
		auto it = rendererInfos.find(rendererName);
		if (it == rendererInfos.end()) return std::nullopt;

		auto matIt = materials.find(it->second);
		if (matIt == materials.end()) return std::nullopt;

		return std::ref(matIt->second);
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

	// TODO set param to all of collection if exists (exemple setLight)
	void setParams(const std::string name) {

	}

	private:
	std::map<std::string, MaterialInstance>& materials; // ref to all materials map
	std::map<std::string, std::string> rendererInfos; // key: renderer type, value: renderer id, allow to filter materials

};