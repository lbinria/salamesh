#pragma once
#include "renderer.h"

struct MeshRenderer : public Renderer {
	
	MeshRenderer(Shader shader) : 
		Renderer(std::move(shader))
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
		setColor(glm::vec3(j["color"][0].get<float>(), j["color"][1].get<float>(), j["color"][2].get<float>()));
		setMeshSize(j["meshSize"].get<float>());
		setMeshShrink(j["meshShrink"].get<float>());
		setCornerVisible(j["isCornerVisible"].get<bool>());
	}

	virtual void doSaveState(json &j) const override {
		j["color"] = json::array({color.x, color.y, color.z});
		j["meshSize"] = meshSize;
		j["meshShrink"] = meshShrink;
		j["isCornerVisible"] = isCornerVisible;
	}

	protected:
	
	float meshSize = 0.01f;
	float meshShrink = 0.f;
	bool isCornerVisible = false;
	glm::vec3 color{0.71f, 0.71f, 0.71f};
	
};