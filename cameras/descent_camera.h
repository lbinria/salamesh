#pragma once

#include "../core/camera.h"

struct DescentCamera final : public Camera {

	using Camera::Camera;

    void updateProjectionMatrix() override {
        float fov = getFov();
        m_projectionMatrix = glm::perspective(glm::radians(fov), _screen.x / _screen.y, nearPlane, farPlane);
    }

    void lookAtBox(std::tuple<glm::vec3, glm::vec3> box) override {
        auto [min, max] = box;
        auto c = (min + max) * .5f;
        m_eye = {c.x, c.y, c.z + glm::length(max - min)};
        m_lookAt = c;
	    updateViewMatrix();
    }

    void move(glm::vec2 oldPos, glm::vec2 newPos) override {
        if (m_lock)
            return;

        glm::vec2 mouseDelta = newPos - oldPos;
        // Compute delta angles from viewport dimensions
        glm::vec2 delta((2.f * M_PI) / _screen.x, M_PI / _screen.y);
        // Compute actual angles move
        float angleX = -mouseDelta.x * delta.x;
        float angleY = -mouseDelta.y * delta.y;

		// 2) YAW: rotate around up‐vector
		glm::mat4 yawMat = glm::rotate(glm::mat4(1.0f), angleX, m_upVector);
		glm::vec3 forwardY = glm::vec3(yawMat * glm::vec4(getViewDir(), 0.0f));

		// 3) Recompute right axis after yaw
		glm::vec3 rightAxis = glm::normalize(glm::cross(forwardY, m_upVector));

		// 4) PITCH: rotate the yawed‐forward around the camera’s right vector
		glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0f), angleY, rightAxis);
		glm::vec3 forwardYP = glm::vec3(pitchMat * glm::vec4(forwardY, 0.0f));

		// 5) Build new lookAt and view‐matrix
		glm::vec3 newLookAt = m_eye + (forwardYP * 1000.0f /* multiply is a quick & dirty fix. It enables look at to be very far away, therefore position doesn't collide with look at */);
        // Note that if there is collision between position & look at (look at is very close to eye), move forward direction gets inverted each times you pass the look at point.
        // This fix doesn't works as soon as you move very close to the look at point, but it's good enough for now.
		glm::mat4 view = glm::lookAt(m_eye, newLookAt, m_upVector);


        // Update camera
        setCameraView(m_eye, newLookAt);
    }

    void moveRight(float speed) override {
        if (m_lock)
            return;

        m_eye += getRightVector() * speed;
        m_lookAt += getRightVector() * speed;
        updateViewMatrix();
    }

    void moveForward(float speed) override {
        if (m_lock)
            return;

        m_eye += getViewDir() * speed;
        updateViewMatrix();
    }

    void moveUp(float speed) override {
        if (m_lock)
            return;

        m_eye += m_upVector * speed;
        m_lookAt += m_upVector * speed;
        updateViewMatrix();
    }

    // void zoom(float delta) {
    //     float factor = sigmoid(_fov, 45.f, 30.f, 2.5f);
    //     _fov = std::clamp(_fov + delta * factor, 0.25f, 60.f);
    //     updateProjectionMatrix();
    // }

    void zoom(float delta) {
        float factor = sigmoid(_zoomFactor, 0.8f, 0.2f, 0.08f /* factor (max slope of sigmoid) */);
        // Clamp zoom factor to be between 0.0055 and 1.33 to have fov that varies between 0.25° / 60° and center to 45°        
        _zoomFactor = std::clamp(_zoomFactor + delta * factor, 0.f, 1.f);
        updateProjectionMatrix();
    }

    void resetZoom() {
        _zoomFactor = 1.f;
        updateProjectionMatrix();
    }

    float getFov() const {
        // zoom factor = 0 -> fov = 0.25° 
        // zoom factor = 1 -> fov = 45°
        return _zoomFactor * FOV_RANGE + FOV_MIN; 
    }
    
    void setFov(float fov) { 
        _zoomFactor = fov / FOV_RANGE - FOV_MIN;
    }

    void doSaveState(json &j) {}
    void doLoadState(json &j) {}

    std::string getType() override { return "DescentCamera"; }

    const float FOV_MIN = .25f;
    const float FOV_MAX = 45.f;
    const float FOV_RANGE = FOV_MAX - FOV_MIN;
};