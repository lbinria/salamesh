#pragma once

#include "../core/camera.h"

struct DescentCamera final : public Camera {

	using Camera::Camera;

    glm::mat4 getProjection() const override {
        return glm::perspective(glm::radians(_fov), _screen.x / _screen.y, nearPlane, farPlane);
    }

    void move(glm::vec2 viewportDims, glm::vec2 mousePos, glm::vec2 lastMousePos) override {
        if (m_lock)
            return;

        // Compute delta angles from viewport dimensions
        glm::vec2 delta((2.f * M_PI) / viewportDims.x, M_PI / viewportDims.y);
        // Compute actual angles move
        float angleX = (lastMousePos.x - mousePos.x) * delta.x;
        float angleY = (lastMousePos.y - mousePos.y) * delta.y;

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
        setCameraView(m_eye, newLookAt, m_upVector, m_projectionMatrix);
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

    void zoom(float delta) {
        float factor = sigmoid(_fov, 45.f, 30.f, 2.5f);
        _fov = std::clamp(_fov + delta * factor, 0.25f, 60.f);
        // setCameraView(m_eye, m_lookAt, m_upVector, getProjection());
        m_projectionMatrix = getProjection();
    }

    void resetZoom() {
        // TODO review this part
        setFov(45.f);
        updateViewMatrix();
    }

    void doSaveState(json &j) {}
    void doLoadState(json &j) {}

    std::string getType() override { return "DescentCamera"; }

};