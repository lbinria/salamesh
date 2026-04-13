#pragma once
#include "shader_params.h"
#include <optional>
#include <variant>

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

	template<class TParam>
	void addParam(const std::string name) {
		params[name] = std::make_unique<TParam>();
	}

	bool hasParam(const std::string name) {
		return params.contains(name);
	}

	template<class TBuffers>
	void addBuffers(const std::string name) {
		buffers[name] = std::make_unique<TBuffers>();
	}

	bool hasBuffers(const std::string name) {
		return buffers.contains(name);
	}

	std::optional<std::reference_wrapper<ShaderParams>> getParams(const std::string name) {
		auto it = params.find(name);
		if (it == params.end()) return std::nullopt;
		return std::ref(*it->second);
	}

	std::optional<std::reference_wrapper<ShaderBuffers>> getBuffers(const std::string name) {
		auto it = buffers.find(name);
		if (it == buffers.end()) return std::nullopt;
		return std::ref(*it->second);
	}

	// TODO add saveState, loadState



	private:
	bool visible = true;
	std::map<std::string, std::unique_ptr<ShaderParams>> params;
	std::map<std::string, std::unique_ptr<ShaderBuffers>> buffers;

};

// Proxy
// Group materials of given renderers together to make some operations
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

	// Set value for all params of a material collection if exists (exemple ("light", "enabled", true))
	void set(const std::string paramsName, const std::string paramName, ShaderParams::ParamValue val) {
		for (auto &[rn, rid] : rendererInfos) {
			auto paramsRef = materials[rid].getParams(paramsName);
			if (paramsRef.has_value()) {
				paramsRef.value().get().set(paramName, val);
			}
		}
	}

	// Check whether all param of name paramName in params group paramsName are true
	// If any value is not bool, return false
	bool all(const std::string paramsName, const std::string paramName) {
		for (auto &[rn, rid] : rendererInfos) {
			auto paramsRef = materials[rid].getParams(paramsName);
			if (!paramsRef.has_value()) {
				continue;
			}
			auto pval = paramsRef.value().get().get(paramName);
			bool *val = std::get_if<bool>(&pval);
			if (!val || !(*val)) 
				return false;

		}

		return true;
	}

	// Check whether any param of name paramName in params group paramsName is true
	// If any value is not bool, return false
	bool any(const std::string paramsName, const std::string paramName) {
		for (auto &[rn, rid] : rendererInfos) {
			auto paramsRef = materials[rid].getParams(paramsName);
			if (!paramsRef.has_value()) {
				continue;
			}
			auto pval = paramsRef.value().get().get(paramName);
			bool *val = std::get_if<bool>(&pval);
			if (!val) 
				return false;
			if (*val)
				return true;
		}

		return false;
	}

	private:
	std::map<std::string, MaterialInstance>& materials; // ref to all materials map
	std::map<std::string, std::string> rendererInfos; // key: renderer type, value: renderer id, allow to filter materials

};