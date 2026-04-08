#pragma once
#include "shader_params.h"

struct MaterialInstance {


	bool getVisible() const { return visible; }


	void apply(Shader &shader) const {
		if (!visible)
			return;
		
		for (auto &[k, p] : params) { 
			p->update(shader);
		}

		for (auto &[k, b] : buffers) {
			b->update(shader);
		}
	}

	// TODO add saveState, loadState
	std::shared_ptr<ShaderParams> getParam(const std::string name) const {
		return params.at(name);
	}

	void addParam(const std::string name, std::shared_ptr<ShaderParams> param) {
		params[name] = param;
	}

	bool hasParam(const std::string name) {
		return params.contains(name);
	}

	template<typename T>
	std::shared_ptr<T> getParam(const std::string name) {
		return std::dynamic_pointer_cast<T>(params.at(name));
	}

	void addBufferGroup(const std::string name, std::shared_ptr<ShaderBufferGroup> bufferGroup) {
		buffers[name] = bufferGroup;
	}

	private:
	bool visible = true;
	std::map<std::string, std::shared_ptr<ShaderParams>> params;
	std::map<std::string, std::shared_ptr<ShaderBufferGroup>> buffers;

};