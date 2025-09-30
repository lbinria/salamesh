#pragma once 
#include <ultimaille/all.h>
#include "../include/glm/glm.hpp"

namespace sl {

	glm::vec2 um2glm(UM::vec2 v) {
		return glm::vec2(v.x, v.y);
	}

	glm::vec3 um2glm(UM::vec3 v) {
		return glm::vec3(v.x, v.y, v.z);
	}

	glm::vec4 um2glm(UM::vec4 v) {
		return glm::vec4(v[0], v[1], v[2], v[3]);
	}

	UM::vec2 glm2um(glm::vec2 v) {
		return {v.x, v.y};
	}

	UM::vec3 glm2um(glm::vec3 v) {
		return {v.x, v.y, v.z};
	}

	UM::vec4 glm2um(glm::vec4 v) {
		return {v.x, v.y, v.z, v.w};
	}
}