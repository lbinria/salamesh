#pragma once
#include <map>
#include <string>
#include "material.h"

struct MaterialParamsGroup {

	MaterialParamsGroup(const std::string paramName, std::map<std::string, Material> &materials) :
		_materials(materials),
		_paramName(paramName)
	{}

	std::vector<ParamValue> get(const std::string name) {
		std::vector<ParamValue> values;

		for (auto &[_, material] : _materials) {
			auto params = material.getParams(_paramName);
			if (!params)
				continue;

			auto paramValue = params->get(name);
			values.push_back(paramValue);
		}

		return values;
	}

	std::vector<bool> getBools(const std::string name) {
		std::vector<bool> result;
		auto values = get(name);
		for (auto &v : values) {
			auto *b = std::get_if<bool>(&v);
			if (b)
				result.push_back(*b);
		}
		return result;
	}

	void set(const std::string name, ParamValue value) {
		for (auto &[_, material] : _materials) {
			auto params = material.getParams(_paramName);
			if (!params)
				continue;

			params->set(name, value);
		}
	}

	private:
	std::string _paramName;
	std::map<std::string, Material>& _materials;
};