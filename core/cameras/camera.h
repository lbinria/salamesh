#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include "helpers.h"

#include <string>
#include <algorithm>
#include <iostream>

struct Camera {

	Camera (const Camera&) = delete;
	Camera& operator= (const Camera&) = delete;

	Camera(std::string name) : _name(name.empty() ? sl::generateGuid() : name) {}

	template<typename T>
	T& as() {
		static_assert(std::is_base_of_v<Camera, T>, "Camera::as() can only be used with derived classes of Camera");
		auto &x = static_cast<T&>(*this);
		return x;
	}

	void setCameraView(vec3 eye, vec3 lookAt)
	{
		_pos = std::move(eye);
		_lookAt = std::move(lookAt);
		updateViewMatrix();
	}

	void setCameraProjection(mat4x4 proj) {
		_proj = std::move(proj);
	}

	virtual void updateProjectionMatrix() = 0;

	// TODO remove from camera move into specific camera if needed
	void updateViewMatrix()
	{
		_view = sl::lookAt(_pos, _lookAt, _up);
	}

	virtual void lookAtBox(std::tuple<vec3, vec3> box) = 0;
	virtual void move(vec2 oldPos, vec2 newPos) = 0;
	virtual void moveRight(double speed) = 0;
	virtual void moveForward(double speed) = 0;
	virtual void moveUp(double speed) = 0;

	virtual void zoom(float delta) = 0;
	virtual void resetZoom() = 0;


	void setLock(bool lock) { _isLocked = lock;}
	bool isLocked() { return _isLocked; }

	float getNearPlane() { return _nearPlane; }
	void setNearPlane(float val) { _nearPlane = val; }

	float getFarPlane() { return _farPlane; }
	void setFarPlane(float val) { _farPlane = val; }


	vec3 getEye() const { return _pos; }
	// TODO maybe make private
	void setEye(vec3 eye) { _pos = std::move(eye); updateViewMatrix(); }

	vec3 getLookAt() const { return _lookAt; }

	// TODO maybe make private
	void lookAt(vec3 lookAt) {
		_lookAt = lookAt;
		updateViewMatrix();
	}

	mat4x4 getViewMatrix() const { return _view; }
	mat4x4 getProjectionMatrix() const { return _proj; }
	vec3 getWorldUpVector() const { return _up; }

	float getZoom() const { return 1.f - _zoomFactor; }
	
	void setZoom(float val) { 
		_zoomFactor = 1.f - val;
		updateProjectionMatrix();
	}
	
	void updateScreenSize(double width, double height) {
		_screen = {width, height}; 
		updateProjectionMatrix();
	}

	vec3 getRightVector() const { 
		return sl::vec4to3(_view.transpose()[0]);
	}
	
	vec3 getUpVector() const { 
		return sl::vec4to3(_view.transpose()[1]);
	}

	vec3 getViewDir() const { 
		return -sl::vec4to3(_view.transpose()[2]);
	}

	void saveState(json &j) {
		j["name"] = _name;
		j["zoom_factor"] = _zoomFactor;
		j["view"] = json::array({
			_view[0][0], _view[0][1], _view[0][2], _view[0][3],
			_view[1][0], _view[1][1], _view[1][2], _view[1][3],
			_view[2][0], _view[2][1], _view[2][2], _view[2][3],
			_view[3][0], _view[3][1], _view[3][2], _view[3][3]
		});

		j["proj"] = json::array({
			_proj[0][0], _proj[0][1], _proj[0][2], _proj[0][3],
			_proj[1][0], _proj[1][1], _proj[1][2], _proj[1][3],
			_proj[2][0], _proj[2][1], _proj[2][2], _proj[2][3],
			_proj[3][0], _proj[3][1], _proj[3][2], _proj[3][3]
		});

		// j["look_at"] = json::array({_lookAt.x, _lookAt.y, _lookAt.z});
		j["type"] = getType();
		doSaveState(j);
	}

	void loadState(json &j) {
		_name = j["name"].get<std::string>();
		_zoomFactor = j["zoom_factor"];
		auto &jView = j["view"];
		_view = mat4x4{
			jView[0].get<double>(), jView[1].get<double>(), jView[2].get<double>(), jView[3].get<double>(),
			jView[4].get<double>(), jView[5].get<double>(), jView[6].get<double>(), jView[7].get<double>(),
			jView[8].get<double>(), jView[9].get<double>(), jView[10].get<double>(), jView[11].get<double>(),
			jView[12].get<double>(), jView[13].get<double>(), jView[14].get<double>(), jView[15].get<double>()
		};

		auto &jProj = j["proj"];
		_proj = mat4x4{
			jProj[0].get<double>(), jProj[1].get<double>(), jProj[2].get<double>(), jProj[3].get<double>(),
			jProj[4].get<double>(), jProj[5].get<double>(), jProj[6].get<double>(), jProj[7].get<double>(),
			jProj[8].get<double>(), jProj[9].get<double>(), jProj[10].get<double>(), jProj[11].get<double>(),
			jProj[12].get<double>(), jProj[13].get<double>(), jProj[14].get<double>(), jProj[15].get<double>()
		};

		// Extract position from view matrix
		mat4x4 c = _view.invert();
		_pos = sl::vec4to3(c[3]);
		// Extract look at from view matrix
		_lookAt = {_view[0][2], _view[1][2], _view[2][2]};

		doLoadState(j);
	}

	// TODO here use box of copied camera 
	void copy(Camera &c, std::tuple<vec3, vec3> box) {
		lookAtBox(box);
		setEye(c.getEye());
		setZoom(c.getZoom());
	}

	virtual void doSaveState(json &j) = 0;
	virtual void doLoadState(json &j) = 0;

	virtual std::string getType() = 0;

	std::string getName() { return _name; }

	protected:
	
	float _zoomFactor = 1.f;
	vec2 _screen;
	mat4x4 _view;
	mat4x4 _proj;
	vec3 _pos; // Camera position in 3D
	vec3 _lookAt; // Point that the camera is looking at
	const vec3 _up{0.f, 1.f, 0.f}; // Orientation of the camera
	bool _isLocked = false;

	float _farPlane = 100.f;
	float _nearPlane = 0.1f;

	/**
	 * @brief Sigmoid function for smooth zooming.
	 */
	float sigmoid(float x, float center=45.f, float w = 90.f, float max_value = 1.f) {
		return (1.f / (1.f + std::exp(-(x - center) / w))) * max_value * 2.;
	}

	private:
	std::string _name;
};