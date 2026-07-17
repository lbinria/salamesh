#pragma once

#include <ultimaille/all.h>
#include <string>
#include <cctype>

#include <random>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace fs = std::filesystem;
using namespace UM;

namespace sl {

	inline vec3 vec4to3(vec4 v) {
		return {v.data[0], v.data[1], v.data[2]};
	}

	inline vec2 div(vec2 a, vec2 b) {
		return {a.x / b.x, a.y / b.y};
	}

	inline vec2 mul(vec2 a, vec2 b) {
		return {a.x * b.x, a.y * b.y};
	}

	inline vec3 mul(vec3 a, vec3 b) {
		return {a.x * b.x, a.y * b.y, a.z * b.z};
	}

	inline vec3 min(vec3 a, vec3 b) {
		return {
			std::min(a.x, b.x), 
			std::min(a.y, b.y), 
			std::min(a.z, b.z)
		};
	}

	inline vec3 max(vec3 a, vec3 b) {
		return {
			std::max(a.x, b.x), 
			std::max(a.y, b.y), 
			std::max(a.z, b.z)
		};
	}

	inline Quaternion angleAxis(double angle, vec3 ax) {
		double half_angle = angle * 0.5;
		Quaternion q;
		q.v = ax * sin(half_angle);
		q.w = cos(half_angle);
		return q;
	}



	inline vec3 rotatev3(vec3 v, Quaternion q) {
		// Convert vec3 to quaternion (pure quaternion with w=0)
		Quaternion p;
		p.v = v;
		p.w = 0.0;
		
		// Rotate: q * p * q_conjugate
		Quaternion q_conj;
		q_conj.v = -q.v;
		q_conj.w = q.w;
		
		Quaternion result = q * p * q_conj;
		return result.v;
	}

	inline vec4 rotate(vec4 v, Quaternion q) {
		vec3 v3{v.data[0], v.data[1], v.data[2]};
		vec3 res = rotatev3(v3, q);
		return vec4{res.x, res.y, res.z, 0};
	}


	inline mat4x4 ortho(double left, double right, double bottom, double top, double zNear, double zFar) {
		mat4x4 res{{
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1},
		}};
		res[0][0] = 2. / (right - left);
		res[1][1] = 2. / (top - bottom);
		res[2][2] = - 2. / (zFar - zNear);
		res[3][0] = - (right + left) / (right - left);
		res[3][1] = - (top + bottom) / (top - bottom);
		res[3][2] = - (zFar + zNear) / (zFar - zNear);
		return res;
	}

	inline mat4x4 translate(mat4x4 m, vec3 v) {
		mat4x4 res(m);
		res[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
		return res;
	}

	inline mat4x4 lookAt(vec3 eye, vec3 center, vec3 up) {
		vec3 f = (center - eye).normalized();
		vec3 s = cross(f, up).normalized();
		vec3 u = cross(s, f);

		mat4x4 res;
		res[0][0] = s.x;
		res[1][0] = s.y;
		res[2][0] = s.z;
		res[3][0] = 0;
		res[0][1] = u.x;
		res[1][1] = u.y;
		res[2][1] = u.z;
		res[3][1] = 0;
		res[0][2] = -f.x;
		res[1][2] = -f.y;
		res[2][2] = -f.z;
		res[3][2] = 0;
		res[3][0] = -(s * eye);
		res[3][1] = -(u * eye);
		res[3][2] = (f * eye);
		res[3][3] = 1;


		return res;
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
		// TODO refactor, see if nDims should not be found elsewhere, probably before in Model::getAttrData
		// Prepare data
		std::vector<float> data;
		int nDims = 1;

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

	static std::vector<std::string> listDirectory(std::string dirPath, std::string ext = "") {
		if (!fs::exists(dirPath))
			return {};
		
		std::vector<std::string> filenames;
		try {
			for (auto const& dir_entry : fs::directory_iterator(dirPath)) {
				if (!fs::is_regular_file(dir_entry.path()) || 
					ext != "" && dir_entry.path().extension() != ext)
					continue;

				// std::string name = dir_entry.path().stem();
				filenames.push_back(fs::absolute(dir_entry.path()).string());
			}
		}
		catch (const fs::filesystem_error & ex) {
			// throw std::runtime_error("Error occurred during loading latest snapshot. " + ex.what());
			throw std::runtime_error("Error occurred during loading latest snapshot.");
		}

		return filenames;
	}

	#ifdef _WIN32
	static std::string exePath(std::string filename) {
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		return std::filesystem::path(buffer).parent_path().string() + "\\" + filename;
	}
	#else
	static std::string exePath(std::string filename) {
		return std::filesystem::canonical("/proc/self/exe").parent_path().string() + "/" + filename;
	}
	#endif



	static std::string shadersPath(std::string filename) {
		return exePath("") + "shaders/" + filename;
	}

	static std::string assetsPath(std::string filename) {
		return exePath("") + "assets/" + filename;
	}

	static std::string generateGuid() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 15);
		std::uniform_int_distribution<> dis2(8, 11);

		std::stringstream ss;
		ss << std::hex;
		
		// Generate 32 hexadecimal digits
		for (int i = 0; i < 32; ++i) {
			if (i == 8 || i == 12 || i == 16 || i == 20)
				ss << '-';
			
			// Variant and version bits
			if (i == 16) {
				ss << dis2(gen);
			} else {
				ss << dis(gen);
			}
		}
		
		return ss.str();
	}

}