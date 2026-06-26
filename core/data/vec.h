#pragma once
#include <ultimaille/all.h>

namespace sl::algebra {
	struct vec2 {
		float x, y;
		
		// vec2(float x, float y) : x(x), y(y) {};
		// vec2(const UM::vec2& v) : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)) {}

		float* value_ptr() {
			return &x;
		}
		
		const float* value_ptr() const {
			return &x;
		}
	};

	struct vec3 {
		float x, y, z;

		// vec3(float x, float y, float z) : x(x), y(y), z(z) {};
		// vec3(const UM::vec3& v) : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)), z(static_cast<float>(v.z)) {}

		float* value_ptr() {
			return &x;
		}
		
		const float* value_ptr() const {
			return &x;
		}
	};

	struct mat4x4 {
		float m[16];  // 4x4 = 16 floats in row-major or column-major order
		
		mat4x4(const UM::mat4x4& mat) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					m[i * 4 + j] = static_cast<float>(mat[i][j]);
				}
			}
		}
		
		float* value_ptr() {
			return &m[0];
		}
		
		const float* value_ptr() const {
			return &m[0];
		}
	};
}