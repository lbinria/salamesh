#pragma once
#include "material_params.h"

struct MeshStyleParams : MaterialParams {
	
	void init() override {

	}

	void apply(Shader &shader) override {
		shader.setFloat3("color", color);
		shader.setFloat("meshSize", meshSize);
		shader.setFloat("meshShrink", meshShrink);
		shader.setInt("isCornerVisible", isCornerVisible);
	}

	ParamValue get(const std::string name) override {
		if (name == "color") {
			return color;
		} else if (name == "size") {
			return meshSize;
		} else if (name == "shrink") {
			return meshShrink;
		} else if (name == "corner_visible") {
			return isCornerVisible;
		} else {
			return 0.f;
		}
	}

	void set(const std::string name, ParamValue value) override {
		if (name == "color") {
			if (auto* pVal = std::get_if<sl::algebra::vec3>(&value))
				color = *pVal;
		} else if (name == "size") {
			if (auto* pVal = std::get_if<float>(&value))
				meshSize = *pVal;
		} else if (name == "shrink") {
			if (auto* pVal = std::get_if<float>(&value))
				meshShrink = *pVal;
		} else if (name == "corner_visible") {
			if (auto* pVal = std::get_if<bool>(&value))
				isCornerVisible = *pVal;
		}
	}

	ParamValue getIndex(const std::string name, int index) override { return 0.f; }
	void setIndex(const std::string name, int index, ParamValue value) override {}

	void loadState(json &j) {
		color = {j["color"][0].get<float>(), j["color"][1].get<float>(), j["color"][2].get<float>()};
		meshSize = j["size"].get<float>();
		meshShrink = j["shrink"].get<float>();
		isCornerVisible = j["corner_visible"].get<bool>();
	}

	void saveState(json &j) const {
		j["color"] = json::array({color.x, color.y, color.z});
		j["size"] = meshSize;
		j["shrink"] = meshShrink;
		j["corner_visible"] = isCornerVisible;
	}


	sl::algebra::vec3 color{0.71f, 0.71f, 0.71f};
	float meshSize = 0.f;
	float meshShrink = 0.f;
	bool isCornerVisible = false;
};