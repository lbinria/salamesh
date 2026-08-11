#pragma once
#include "shader.h"


#include "json.hpp"
using json = nlohmann::json;

#include <variant>

#include "clipping_mode.h"

using ParamValue = std::variant<bool, int, float, sl::algebra::vec2, sl::algebra::vec3>;

// struct MaterialParams {
// 	virtual void init() = 0;
// 	virtual void apply(Shader &shader) = 0;


// 	virtual ParamValue get(const std::string name) = 0;
// 	virtual void set(const std::string name, ParamValue value) = 0;



// 	virtual void loadState(json &j) = 0;
// 	virtual void saveState(json &j) const = 0;

// 	virtual void setValues(MaterialParams &params) = 0;

// };

struct MaterialParams {

	MaterialParams() {}

	MaterialParams(const std::string name, std::map<std::string, ParamValue> params) 
		: name(name), _params(params) {}

	static MaterialParams getPointStyleMaterialParams() {
		return {"style", {
			{"size", 4.f},
			{"color", sl::algebra::vec3{0.23f, 0.85f, 0.66f}}
		}};
	}

	static MaterialParams getEdgeStyleMaterialParams() {
		return {"style", {
			{"thickness", 2.f},
			{"spacing", 0.f},
			{"padding", 0.f},
			{"inside_color", sl::algebra::vec3{0.0, 0.97, 0.73}},
			{"outside_color", sl::algebra::vec3{0.0, 0.6, 0.45}}
		}};
	}

	static MaterialParams getStyleMaterialParams() {
		return {"style", {
			{"color", sl::algebra::vec3{0.71f, 0.71f, 0.71f}},
			{"size", 0.f},
			{"shrink", 0.f},
			{"corner_visible", false},
		}};
	}

	static MaterialParams getLightMaterialParams() {
		return {"light", {
			{"enabled", true},
			{"dir", sl::algebra::vec3{-0.5f, -0.8f, 0.2f}}
		}};
	}

	static MaterialParams getClippingMaterialParams() {
		return {"clipping", {
			{"mode", static_cast<int>(ClippingMode::STD)},
			{"enabled", false},
			{"invert", false},
			{"point", sl::algebra::vec3{0.,0.,0.}},
			{"normal", sl::algebra::vec3{0.,1.,0.}}
		}};
	}

	virtual void init() {}

	virtual void apply(Shader &shader) {
		for (auto &[paramName, val] : _params) {
			auto fullname = name + "." + paramName;

			if (auto *pVal = std::get_if<bool>(&val))
				shader.setBool(fullname, *pVal);
			else if (auto *pVal = std::get_if<int>(&val))
				shader.setInt(fullname, *pVal);
			else if (auto *pVal = std::get_if<float>(&val))
				shader.setFloat(fullname, *pVal);
			else if (auto *pVal = std::get_if<sl::algebra::vec2>(&val))
				shader.setFloat2(fullname, *pVal);
			else if (auto *pVal = std::get_if<sl::algebra::vec3>(&val))
				shader.setFloat3(fullname, *pVal);
		}
	}

	virtual ParamValue get(const std::string name) {
		if (_params.contains(name))
			return _params.at(name);

		return 0.f;
	}

	virtual void set(const std::string name, ParamValue value) {
		_params[name] = value;
	}

	std::string getType(const std::string name) const {
		if (!_params.contains(name))
			return "";
		
		auto val = _params.at(name);

		if (auto *pVal = std::get_if<bool>(&val))
			return "bool";
		else if (auto *pVal = std::get_if<int>(&val))
			return "int";
		else if (auto *pVal = std::get_if<float>(&val))
			return "float";
		else if (auto *pVal = std::get_if<sl::algebra::vec2>(&val))
			return "vec2";
		else if (auto *pVal = std::get_if<sl::algebra::vec3>(&val))
			return "vec3";
	}

	ParamValue& operator[](const std::string& key) {
		return _params[key];
	}

	// beurk !
	virtual void loadState(json &j) {
		for (auto &[paramName, val] : _params) {
			if (auto *pVal = std::get_if<bool>(&val))
				set(paramName, j[paramName].get<bool>());
			else if (auto *pVal = std::get_if<int>(&val))
				set(paramName, j[paramName].get<int>());
			else if (auto *pVal = std::get_if<float>(&val))
				set(paramName, j[paramName].get<float>());
			else if (auto *pVal = std::get_if<sl::algebra::vec2>(&val))
				set(paramName, sl::algebra::vec2{
					j[paramName][0].get<float>(), 
					j[paramName][1].get<float>()
				});
			else if (auto *pVal = std::get_if<sl::algebra::vec3>(&val))
				set(paramName, sl::algebra::vec3{
					j[paramName][0].get<float>(), 
					j[paramName][1].get<float>(), 
					j[paramName][2].get<float>()
				});
		}
	}

	// beurk !
	virtual void saveState(json &j) const {
		for (auto &[paramName, val] : _params) {
			if (auto *pVal = std::get_if<bool>(&val))
				j[paramName] = *pVal;
			else if (auto *pVal = std::get_if<int>(&val))
				j[paramName] = *pVal;
			else if (auto *pVal = std::get_if<float>(&val))
				j[paramName] = *pVal;
			else if (auto *pVal = std::get_if<sl::algebra::vec2>(&val)) {
				auto val = *pVal;
				j[paramName] = json::array({val.x, val.y});
			} else if (auto *pVal = std::get_if<sl::algebra::vec3>(&val)) {
				auto val = *pVal;
				j[paramName] = json::array({val.x, val.y, val.z});
			}
		}
	}

	virtual void setValues(MaterialParams &params) {
		for (auto &[paramName, param] : params._params) {
			set(paramName, params.get(paramName));
		}
	}

	std::string getName() const { return name; }
	std::map<std::string, ParamValue>& getParams() { return _params; }

	private:
	std::string name;
	std::map<std::string, ParamValue> _params;
};