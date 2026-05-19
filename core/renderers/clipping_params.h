#pragma once
#include "material_params.h"
#include "clipping_mode.h"

struct ClippingParams : MaterialParams {
	

	void init() override {

	}

	void apply(Shader &shader) override {
		shader.setInt("clipping_mode", mode);		
		shader.setInt("is_clipping_enabled", enabled);
		shader.setFloat3("clipping_plane_point", point);
		shader.setFloat3("clipping_plane_normal", normal);
		shader.setInt("invert_clipping", invert);

	}

	ParamValue get(const std::string name) override {
		if (name == "mode") {
			return static_cast<int>(mode);
		} else if (name == "enabled") {
			return enabled;
		} else if (name == "point") {
			return point;
		} else if (name == "normal") {
			return normal;
		} else if (name == "invert") {
			return invert;
		} else {
			return 0.f;
		}
	}

	void set(const std::string name, ParamValue value) override {
		if (name == "mode") {
			if (auto* pVal = std::get_if<int>(&value))
				mode = static_cast<ClippingMode>(*pVal);
		} else if (name == "enabled") {
			if (auto* pVal = std::get_if<bool>(&value))
				enabled = *pVal;
		} else if (name == "point") {
			if (auto* pVal = std::get_if<glm::vec3>(&value))
				point = *pVal;
		} else if (name == "normal") {
			if (auto* pVal = std::get_if<glm::vec3>(&value))
				normal = *pVal;
		} else if (name == "invert") {
			if (auto* pVal = std::get_if<bool>(&value))
				invert = *pVal;
		}
	}

	ParamValue get(const std::string name, int index) override { return 0.f; }
	void set(const std::string name, int index, ParamValue value) override {}

	void loadState(json &j) {
		mode = static_cast<ClippingMode>(j["mode"].get<int>());
		enabled = j["enabled"].get<bool>();
		point = glm::vec3(j["point"][0].get<float>(), j["point"][1].get<float>(), j["point"][2].get<float>());
		normal = glm::vec3(j["normal"][0].get<float>(), j["normal"][1].get<float>(), j["normal"][2].get<float>());
		invert = j["invert"].get<bool>();
	}

	void saveState(json &j) const {
		j["mode"] = static_cast<int>(mode);
		j["enabled"] = enabled;
		j["point"] = json::array({point.x, point.y, point.z});
		j["normal"] = json::array({normal.x, normal.y, normal.z});
		j["invert"] = invert;
	}

	ClippingMode mode = ClippingMode::STD;
	bool enabled = false;
	glm::vec3 point{0.,0.,0.};
	glm::vec3 normal{0.,1.,0.};
	bool invert = false;

};