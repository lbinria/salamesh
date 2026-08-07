#pragma once
#include "material_params.h"

struct PointStyleParams : MaterialParams {
	
	void init() override {

	}

	void apply(Shader &shader) override {
		shader.setFloat("style.size", size);
		shader.setFloat3("style.color", color);
	}

	ParamValue get(const std::string name) override {
		if (name == "size") {
			return size;
		} else if (name == "color") {
			return color;
		} else {
			return 0.f;
		}
	}

	void set(const std::string name, ParamValue value) override {
		if (name == "size") {
			if (auto* pSize = std::get_if<float>(&value))
				size = *pSize;
		} else if (name == "color") {
			if (auto* pColor = std::get_if<sl::algebra::vec3>(&value))
				color = *pColor;
		}
	}

	void loadState(json &j) {
		size = j["pointSize"].get<float>();
		color = {j["pointColor"][0].get<float>(), j["pointColor"][1].get<float>(), j["pointColor"][2].get<float>()};
	}

	void saveState(json &j) const {
		j["pointSize"] = size;
		j["pointColor"] = json::array({color.x, color.y, color.z});
	}

	void setValues(MaterialParams &params) override {
		set("size", params.get("size"));
		set("color", params.get("color"));
	}


	float size = 4.f;
	sl::algebra::vec3 color{0.23f, 0.85f, 0.66f};
};