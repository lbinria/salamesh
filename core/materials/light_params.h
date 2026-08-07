#pragma once
#include "material_params.h"

struct LightParams : MaterialParams {
	
	void init() override {

	}

	void apply(Shader &shader) override {
		shader.setBool("light.enabled", enabled);
		shader.setFloat3("light.dir", dir);
	}

	ParamValue get(const std::string name) override {
		if (name == "enabled") {
			return enabled;
		} else if (name == "dir") {
			return dir;
		} else {
			return 0.f;
		}
	}

	void set(const std::string name, ParamValue value) override {
		if (name == "enabled") {
			if (auto* pVal = std::get_if<bool>(&value))
				enabled = *pVal;
		} else if (name == "dir") {
			if (auto* pVal = std::get_if<sl::algebra::vec3>(&value))
				dir = *pVal;
		}
	}

	void loadState(json &j) {
		enabled = j["enabled"].get<bool>();
	}

	void saveState(json &j) const {
		j["enabled"] = enabled;
	}

	void setValues(MaterialParams &params) override {
		set("enabled", params.get("enabled"));
		set("dir", params.get("dir"));
	}

	bool enabled = true;
	sl::algebra::vec3 dir{-0.5f, -0.8f, 0.2f};
	// vec3(0.35,0.45,1.)

};