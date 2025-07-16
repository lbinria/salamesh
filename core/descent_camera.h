/*
* Thanks to https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
*/

#pragma once

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

// const float FAR_PLANE = 100.f;
// const float NEAR_PLANE = 0.1f;

struct DescentCamera {

    DescentCamera() = default;

    DescentCamera(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up, glm::vec3 fovAndScreen) : 
        m_eye(std::move(eye)),
        m_target_eye(m_eye),
        m_lookAt(std::move(lookAt)),
        m_upVector(std::move(up)),
        m_projectionMatrix(glm::perspective(glm::radians(fovAndScreen.x), fovAndScreen.y / fovAndScreen.z, NEAR_PLANE, FAR_PLANE)),
        m_fovAndScreen(std::move(fovAndScreen))
    {
        updateViewMatrix();
    }

    void setCameraView(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up, glm::mat4x4 projection)
    {
        m_eye = std::move(eye);
        m_lookAt = std::move(lookAt);
        m_upVector = std::move(up);
        m_projectionMatrix = std::move(projection);
        updateViewMatrix();
    }

    void setCameraView(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up, glm::vec3 fovAndScreen)
    {
        setCameraView(eye, lookAt, up, glm::perspective(glm::radians(fovAndScreen.x), fovAndScreen.y / fovAndScreen.z, NEAR_PLANE, FAR_PLANE));
        updateViewMatrix();
    }

    void lookAt(glm::vec3 lookAt) {
        setCameraView(m_eye, lookAt, m_upVector, m_projectionMatrix);
    }

    void updateViewMatrix()
    {
        // Generate view matrix using the eye, lookAt and up vector
        m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);
    }

    void move(glm::vec2 viewportDims, glm::vec2 mousePos, glm::vec2 lastMousePos) {
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

    void movePlane(glm::vec2 deltaPos) {
        if (m_lock)
            return;

        glm::vec3 lookAt(m_lookAt.x, m_lookAt.y, m_lookAt.z);
        lookAt -= getRightVector() * deltaPos.x * .005f;
        lookAt += getUpVector() * deltaPos.y * .005f;
        setCameraView(m_eye, lookAt, m_upVector, m_projectionMatrix);
    }

    void moveRight(float speed) {
        if (m_lock)
            return;

        m_eye += getRightVector() * speed;
        m_lookAt += getRightVector() * speed;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void moveForward(float speed) {
        if (m_lock)
            return;

        m_eye += getViewDir() * speed;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void moveUp(float speed) {
        if (m_lock)
            return;

        m_eye += m_upVector * speed;
        m_lookAt += m_upVector * speed;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void zoom(float delta) {
        float factor = sigmoid(m_fovAndScreen.x, 45.f, 30.f, 2.5f);
        m_fovAndScreen.x = std::clamp(m_fovAndScreen.x + delta * factor, 0.25f, 60.f);

        setCameraView(m_eye, m_lookAt, m_upVector, m_fovAndScreen);
    }

    void resetZoom() {
        setFov(45.f);
        updateViewMatrix();
    }

    // Property: lock
    void setLock(bool lock) { m_lock = lock;}
    bool isLocked() { return m_lock; }


    glm::mat4x4 getViewMatrix() const { return m_viewMatrix; }
    glm::mat4x4 getProjectionMatrix() const { return m_projectionMatrix; }

    glm::vec3 getUpVector() const { return m_upVector; }
    glm::vec3 getLookAt() const { return m_lookAt; }

    // Property: position
    glm::vec3 getEye() const { return m_eye; }
    void setEye(glm::vec3 eye) { m_eye = std::move(eye); updateViewMatrix(); }

    // Read-only property: fov_and_screen
    glm::vec3 getFovAndScreen() const { return m_fovAndScreen; }

    // Property: fov
    float getFov() const { return m_fovAndScreen.x; }
    void setFov(float fov) { m_fovAndScreen.x = fov; zoom(0.f);/* TODO here update after set fov... not really smart */ }
    
    void setScreenSize(float width, float height) { m_fovAndScreen.y = width; m_fovAndScreen.z = height; }

    /**
     * @brief Returns the view direction of the camera. 
     * View direction is the negative z-axis of the view matrix.
     * @note Exposed
     */
    glm::vec3 getViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }
    glm::vec3 getRightVector() const { return glm::transpose(m_viewMatrix)[0]; }

private:
    glm::vec3 m_fovAndScreen;
    glm::mat4x4 m_viewMatrix;
    glm::mat4x4 m_projectionMatrix;
    glm::vec3 m_eye; // Camera position in 3D
    glm::vec3 m_lookAt; // Point that the camera is looking at
    glm::vec3 m_upVector; // Orientation of the camera
    glm::vec3 m_target_eye;
    bool m_lock = false;

    /**
     * @brief Sigmoid function for smooth zooming.
     */
    float sigmoid(float x, float center=45.f, float w = 90.f, float max_value = 1.f) {
        return (1.f / (1.f + std::exp(-(x - center) / w))) * max_value * 2.;
    }

};