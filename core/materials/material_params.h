#pragma once
#include "shader.h"


#include "json.hpp"
using json = nlohmann::json;

#include <variant>


using ParamValue = std::variant<bool, int, float, sl::algebra::vec2, sl::algebra::vec3>;

struct MaterialParams {
	virtual void init() = 0;
	virtual void apply(Shader &shader) = 0;


	virtual ParamValue get(const std::string name) = 0;
	virtual void set(const std::string name, ParamValue value) = 0;



	virtual void loadState(json &j) = 0;
	virtual void saveState(json &j) const = 0;

	virtual void setValues(MaterialParams &params) = 0;

};