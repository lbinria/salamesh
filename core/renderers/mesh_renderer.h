#pragma once
#include "renderer.h"

struct MeshMaterial : public Material {
	
	MeshMaterial(std::string name, Shader shader) : 
		Material(name, std::move(shader))
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