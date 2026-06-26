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

	Camera(std::string name) : name(name.empty() ? sl::generateGuid() : name) {}

	template<typename T>
	T& as() {
		static_assert(std::is_base_of_v<Camera, T>, "Camera::as() can only be used with derived classes of Camera");
		auto &x = static_cast<T&>(*this);
		return x;
	}

	void setCameraView(vec3 eye, vec3 lookAt)
	{
		m_eye = std::move(eye);
		m_lookAt = std::move(lookAt);
		updateViewMatrix();
	}

	void setCameraProjection(mat4x4 proj) {
		m_projectionMatrix = std::move(proj);
	}

	virtual void updateProjectionMatrix() = 0;

	// TODO remove from camera move into specific camera if needed
	void updateViewMatrix()
	{
		m_viewMatrix = sl::lookAt(m_eye, m_lookAt, m_upVector);
	}

	virtual void lookAtBox(std::tuple<vec3, vec3> box) = 0;
	virtual void move(vec2 oldPos, vec2 newPos) = 0;
	virtual void moveRight(double speed) = 0;
	virtual void moveForward(double speed) = 0;
	virtual void moveUp(double speed) = 0;

	virtual void zoom(float delta) = 0;
	virtual void resetZoom() = 0;


	void setLock(bool lock) { m_lock = lock;}
	bool isLocked() { return m_lock; }

	float getNearPlane() { return nearPlane; }
	void setNearPlane(float val) { nearPlane = val; }

	float getFarPlane() { return farPlane; }
	void setFarPlane(float val) { farPlane = val; }


	vec3 getEye() const { return m_eye; }
	// TODO maybe make private
	void setEye(vec3 eye) { m_eye = std::move(eye); updateViewMatrix(); }

	vec3 getLookAt() const { return m_lookAt; }

	// TODO maybe make private
	void lookAt(vec3 lookAt) {
		m_lookAt = lookAt;
		updateViewMatrix();
	}

	mat4x4 getViewMatrix() const { return m_viewMatrix; }
	mat4x4 getProjectionMatrix() const { return m_projectionMatrix; }
	vec3 getWorldUpVector() const { return m_upVector; }

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
		return sl::vec4to3(m_viewMatrix.transpose()[0]);
	}
	
	vec3 getUpVector() const { 
		return sl::vec4to3(m_viewMatrix.transpose()[1]);
	}

	vec3 getViewDir() const { 
		return -sl::vec4to3(m_viewMatrix.transpose()[2]);
	}

	void saveState(json &j) {
		j["name"] = m_name;
		j["zoom_factor"] = _zoomFactor;
		j["view"] = json::array({
			m_viewMatrix[0][0], m_viewMatrix[0][1], m_viewMatrix[0][2], m_viewMatrix[0][3],
			m_viewMatrix[1][0], m_viewMatrix[1][1], m_viewMatrix[1][2], m_viewMatrix[1][3],
			m_viewMatrix[2][0], m_viewMatrix[2][1], m_viewMatrix[2][2], m_viewMatrix[2][3],
			m_viewMatrix[3][0], m_viewMatrix[3][1], m_viewMatrix[3][2], m_viewMatrix[3][3]
		});

		j["proj"] = json::array({
			m_projectionMatrix[0][0], m_projectionMatrix[0][1], m_projectionMatrix[0][2], m_projectionMatrix[0][3],
			m_projectionMatrix[1][0], m_projectionMatrix[1][1], m_projectionMatrix[1][2], m_projectionMatrix[1][3],
			m_projectionMatrix[2][0], m_projectionMatrix[2][1], m_projectionMatrix[2][2], m_projectionMatrix[2][3],
			m_projectionMatrix[3][0], m_projectionMatrix[3][1], m_projectionMatrix[3][2], m_projectionMatrix[3][3]
		});

		// j["look_at"] = json::array({m_lookAt.x, m_lookAt.y, m_lookAt.z});
		j["type"] = getType();
		doSaveState(j);
	}

	void loadState(json &j) {
		m_name = j["name"].get<std::string>();
		_zoomFactor = j["zoom_factor"];
		auto &jView = j["view"];
		m_viewMatrix = mat4x4{
			jView[0].get<double>(), jView[1].get<double>(), jView[2].get<double>(), jView[3].get<double>(),
			jView[4].get<double>(), jView[5].get<double>(), jView[6].get<double>(), jView[7].get<double>(),
			jView[8].get<double>(), jView[9].get<double>(), jView[10].get<double>(), jView[11].get<double>(),
			jView[12].get<double>(), jView[13].get<double>(), jView[14].get<double>(), jView[15].get<double>()
		};

		auto &jProj = j["proj"];
		m_projectionMatrix = mat4x4{
			jProj[0].get<double>(), jProj[1].get<double>(), jProj[2].get<double>(), jProj[3].get<double>(),
			jProj[4].get<double>(), jProj[5].get<double>(), jProj[6].get<double>(), jProj[7].get<double>(),
			jProj[8].get<double>(), jProj[9].get<double>(), jProj[10].get<double>(), jProj[11].get<double>(),
			jProj[12].get<double>(), jProj[13].get<double>(), jProj[14].get<double>(), jProj[15].get<double>()
		};

		// Extract position from view matrix
		mat4x4 c = m_viewMatrix.invert();
		m_eye = sl::vec4to3(c[3]);
		// Extract look at from view matrix
		// auto &jLookAt = j["look_at"];
		m_lookAt = {m_viewMatrix[0][2], m_viewMatrix[1][2], m_viewMatrix[2][2]};

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

	std::string getName() { return name; }

	protected:
	std::string m_name;
	float _zoomFactor = 1.f;
	vec2 _screen;
	mat4x4 m_viewMatrix;
	mat4x4 m_projectionMatrix;
	vec3 m_eye; // Camera position in 3D
	vec3 m_lookAt; // Point that the camera is looking at
	const vec3 m_upVector{0.f, 1.f, 0.f}; // Orientation of the camera
	bool m_lock = false;

	float farPlane = 100.f;
	float nearPlane = 0.1f;

	/**
	 * @brief Sigmoid function for smooth zooming.
	 */
	float sigmoid(float x, float center=45.f, float w = 90.f, float max_value = 1.f) {
		return (1.f / (1.f + std::exp(-(x - center) / w))) * max_value * 2.;
	}

	private:
	std::string name;
};