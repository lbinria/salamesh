#pragma once
#include "shader_base.h"

struct MeshMaterial : public ShaderBase {
	
	MeshMaterial(std::string name, Shader shader) : 
		ShaderBase(name, std::move(shader))
		{
			shader.use();
			shader.setFloat3("color", {0.71f, 0.71f, 0.71f});
		}

	void setMeshIndex(int index) {
		shader.use();
		shader.setInt("meshIndex", index);
	}

	virtual void doLoadState(json &j) override {
		for (auto &[paramsName, params] : _params) {
			if (j.contains(paramsName))
				params->loadState(j[paramsName]);
		}
	}

	virtual void doSaveState(json &j) const override {
		for (auto &[paramsName, params] : _params) {
			j[paramsName] = json::object();
			params->saveState(j[paramsName]);
		}
	}

	protected:
	
};