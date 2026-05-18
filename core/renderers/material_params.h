#pragma once
#include "../shader.h"

#include "../../include/glm/glm.hpp"

#include "../../include/json.hpp"
using json = nlohmann::json;

#include <variant>


using ParamValue = std::variant<float, int, glm::vec2, glm::vec3>;
struct MaterialParams {
	virtual void init() = 0;
	virtual void apply(Shader &shader) = 0;


	virtual ParamValue get(const std::string name) = 0;
	virtual void set(const std::string name, ParamValue value) = 0;
	virtual ParamValue get(const std::string name, int index) = 0;
	virtual void set(const std::string name, int index, ParamValue value) = 0;


	virtual void loadState(json &j) = 0;
	virtual void saveState(json &j) const = 0;

};