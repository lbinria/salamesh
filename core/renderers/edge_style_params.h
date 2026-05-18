#pragma once
#include "material_params.h"

struct EdgeStyleParams : MaterialParams {
	
	void init() override {

	}

	void apply(Shader &shader) override {
		shader.setFloat("thickness", size);
		shader.setFloat("spacing", spacing);
		shader.setFloat("padding", padding);
		shader.setFloat3("uColorInside", insideColor);
		shader.setFloat3("uColorOutside", outsideColor);
	}

	ParamValue get(const std::string name) override {
		if (name == "thickness") {
			return size;
		} else if (name == "spacing") {
			return spacing;
		} else if (name == "padding") {
			return padding;
		} else if (name == "inside_color") {
			return insideColor;
		} else if (name == "outside_color") {
			return outsideColor;
		} else {
			return 0.f;
		}
	}

	void set(const std::string name, ParamValue value) override {
		if (name == "thickness") {
			if (auto* pVal = std::get_if<float>(&value))
				size = *pVal;
		} else if (name == "spacing") {
			if (auto* pVal = std::get_if<float>(&value))
				spacing = *pVal;
		} else if (name == "padding") {
			if (auto* pVal = std::get_if<float>(&value))
				padding = *pVal;
		} else if (name == "inside_color") {
			if (auto* pVal = std::get_if<glm::vec3>(&value))
				insideColor = *pVal;
		} else if (name == "outside_color") {
			if (auto* pVal = std::get_if<glm::vec3>(&value))
				outsideColor = *pVal;
		}
	}

	ParamValue get(const std::string name, int index) override { return 0.f; }
	void set(const std::string name, int index, ParamValue value) override {}

	void loadState(json &j) {
		size = j["thickness"].get<float>();
		spacing = j["spacing"].get<float>();
		padding = j["padding"].get<float>();
		insideColor = {j["inside_color"][0].get<float>(), j["inside_color"][1].get<float>(), j["inside_color"][2].get<float>()};
		outsideColor = {j["outside_color"][0].get<float>(), j["outside_color"][1].get<float>(), j["outside_color"][2].get<float>()};
	}

	void saveState(json &j) const {
		j["thickness"] = size;
		j["spacing"] = spacing;
		j["padding"] = padding;
		j["inside_color"] = json::array({insideColor.x, insideColor.y, insideColor.z});
		j["outside_color"] = json::array({outsideColor.x, outsideColor.y, outsideColor.z});
	}

	float size = 2.f;
	float spacing = 0.f;
	float padding = 0.f;
	glm::vec3 insideColor{0.0, 0.97, 0.73};
	glm::vec3 outsideColor{0.0, 0.6, 0.45};
};