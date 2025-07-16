#pragma once

#include "core/camera.h"

struct DescentCamera final : public Camera {

	using Camera::Camera;

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
		glm::vec3 newLookAt = m_eye + forwardYP;
		glm::mat4 view = glm::lookAt(m_eye, newLookAt, m_upVector);


        // Update camera
        setCameraView(m_eye, newLookAt, m_upVector, m_projectionMatrix);
    }

    void moveRight(float speed) override {
        if (m_lock)
            return;

        m_eye += getRightVector() * speed;
        m_lookAt += getRightVector() * speed;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void moveForward(float speed) override {
        if (m_lock)
            return;

        m_eye += getViewDir() * speed;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void moveUp(float speed) override {
        if (m_lock)
            return;

        m_eye += m_upVector * speed;
        m_lookAt += m_upVector * speed;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

};