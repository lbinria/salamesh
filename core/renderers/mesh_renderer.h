#pragma once
#include "renderer.h"

struct MeshMaterial : public Material {
	
	MeshMaterial(std::string name, Shader shader) : 
		Material(name, std::move(shader))
		{
			shader.use();
			shader.setFloat3("color", {0.71f, 0.71f, 0.71f});
		}

	float getMeshSize() const {
		return meshSize;
	}

	void setMeshSize(float val) {
		shader.use();
		shader.setFloat("meshSize", val);
		meshSize = val;
	}

	float getMeshShrink() const {
		return meshShrink;
	}

	void setMeshShrink(float val) {
		shader.use();
		shader.setFloat("meshShrink", val);
		meshShrink = val;
	}

	bool getCornerVisible() const {
		return isCornerVisible;
	}

	void setCornerVisible(bool val) {
		shader.use();
		shader.setInt("isCornerVisible", val);
		isCornerVisible = val;
	}

	void setMeshIndex(int index) {
		shader.use();
		shader.setInt("meshIndex", index);
	}

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("color", c);
		color = c;
	}

	virtual void doLoadState(json &j) override {
		for (auto &[paramsName, params] : _params) {
			params->loadState(j);
		}
	}

	virtual void doSaveState(json &j) const override {
		for (auto &[paramsName, params] : _params) {
			params->saveState(j);
		}
	}

	protected:
	
	float meshSize = 0.01f;
	float meshShrink = 0.f;
	bool isCornerVisible = false;
	glm::vec3 color{0.71f, 0.71f, 0.71f};
	
};