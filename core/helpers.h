#pragma once

#include <ultimaille/all.h>
#include "../include/glm/glm.hpp"
#include <string>
#include <cctype>

namespace sl {

	inline glm::vec2 um2glm(UM::vec2 v) {
		return glm::vec2(v.x, v.y);
	}

	inline glm::vec3 um2glm(UM::vec3 v) {
		return glm::vec3(v.x, v.y, v.z);
	}

	inline glm::vec4 um2glm(UM::vec4 v) {
		return glm::vec4(v[0], v[1], v[2], v[3]);
	}

	inline UM::vec2 glm2um(glm::vec2 v) {
		return {v.x, v.y};
	}

	inline UM::vec3 glm2um(glm::vec3 v) {
		return {v.x, v.y, v.z};
	}

	inline UM::vec4 glm2um(glm::vec4 v) {
		return {v.x, v.y, v.z, v.w};
	}

	inline void toLower(std::string &s) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
	}

	inline std::tuple<float, float> getRange(std::vector<float>& data, int dim = 0, int nDims = 1) {
		float min = std::numeric_limits<float>::max(); 
		float max = std::numeric_limits<float>::min();
		for (int i = dim; i < data.size(); i+=nDims) {
			auto x = data[i];
			min = std::min(min, x);
			max = std::max(max, x);
		}

		return std::make_tuple(min, max);
	}

	template<typename T>
	inline void mapSingleDim(std::vector<T>& data, std::vector<float>& result) {
		result.resize(data.size());
		for (int i = 0; i < data.size(); ++i) {
			result[i] = static_cast<float>(data[i]);
		}
	}

	template<typename T, int nDims>
	void mapManyDims(std::vector<T>& data, int selectedDim, std::vector<float>& result) {
		// YOu can refactor this !
		if (selectedDim == -1) {
			// Inline data
			result.resize(data.size() * nDims);
			for (int i = 0; i < data.size(); ++i) {
				for (int d = 0; d < nDims; ++d) {
					result[i * nDims + d] = static_cast<float>(data[i][d]);
				}
			}
		} else {
			result.resize(data.size());
			for (int i = 0; i < data.size(); ++i) {
				result[i] = static_cast<float>(data[i][selectedDim]);
			}
		}
	}

	inline std::vector<float> getContainerData(ContainerBase *ga, int selectedDim) {
		// Prepare data
		std::vector<float> data;

		// Transform data
		if (auto a = dynamic_cast<AttributeContainer<double>*>(ga)) {
			mapSingleDim<double>(a->data, data);
		} else if (auto a = dynamic_cast<AttributeContainer<float>*>(ga)) {
			mapSingleDim<float>(a->data, data);
		} else if (auto a = dynamic_cast<AttributeContainer<int>*>(ga)) {
			mapSingleDim<int>(a->data, data);
		} else if (auto a = dynamic_cast<AttributeContainer<bool>*>(ga)) {
			mapSingleDim<bool>(a->data, data);
		} else if (auto a = dynamic_cast<AttributeContainer<vec2>*>(ga)) {
			mapManyDims<vec2, 2>(a->data, selectedDim, data);
		} else if (auto a = dynamic_cast<AttributeContainer<vec3>*>(ga)) {
			mapManyDims<vec3, 3>(a->data, selectedDim, data);
		} else {
			throw std::runtime_error("Attribute type is not supported in `getContainerData`.");
		}
		
		return data;
	}

}