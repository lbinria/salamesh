#pragma once
#include "material_params.h"

struct LightParams : MaterialParams {
	
	void init() override {

	}

	void apply(Shader &shader) override {
		shader.setFloat("is_light_enabled", static_cast<float>(enabled)); // TODO set int here !
	}

	ParamValue get(const std::string name) override {
		 if (name == "enabled") {
			return enabled;
		} else {
			return 0.f;
		}
	}

	void set(const std::string name, ParamValue value) override {
		if (name == "enabled") {
			if (auto* pVal = std::get_if<bool>(&value))
				enabled = *pVal;
		}
	}

	ParamValue get(const std::string name, int index) override { return 0.f; }
	void set(const std::string name, int index, ParamValue value) override {}

	void loadState(json &j) {

	}

	void saveState(json &j) const {

	}

	bool enabled = false;

};