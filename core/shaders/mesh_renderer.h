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

	}

	virtual void doSaveState(json &j) const override {

	}

	protected:
	
};