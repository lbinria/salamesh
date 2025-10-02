#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#include <string>
#include <algorithm>

struct Camera {

	const float FAR_PLANE = 100.f;
	// const float NEAR_PLANE = 0.001f;
	const float NEAR_PLANE = 0.1f;

    Camera() = default;

    Camera(std::string name, glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up, glm::vec3 fovAndScreen) : 
		m_name(name),
        m_eye(std::move(eye)),
        m_target_eye(m_eye),
        m_lookAt(std::move(lookAt)),
        m_upVector(std::move(up)),
        m_projectionMatrix(glm::perspective(glm::radians(fovAndScreen.x), fovAndScreen.y / fovAndScreen.z, NEAR_PLANE, FAR_PLANE)),
        // m_projectionMatrix(glm::ortho(-1.f, 1.f, -1.f, 1.f, NEAR_PLANE, FAR_PLANE)),
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
        // setCameraView(eye, lookAt, up, glm::ortho(-1.f, 1.f, -1.f, 1.f, NEAR_PLANE, FAR_PLANE));
        updateViewMatrix();
    }

    void updateViewMatrix()
    {
        // Generate view matrix using the eye, lookAt and up vector
        m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);
    }

    virtual void move(glm::vec2 viewportDims, glm::vec2 mousePos, glm::vec2 lastMousePos) = 0;
    virtual void moveRight(float speed) = 0;
    virtual void moveForward(float speed) = 0;
    virtual void moveUp(float speed) = 0;

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

	// Property: name
	const std::string& getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }

    // Property: position
    glm::vec3 getEye() const { return m_eye; }
    void setEye(glm::vec3 eye) { m_eye = std::move(eye); updateViewMatrix(); }

    // Property: look_at
    glm::vec3 getLookAt() const { return m_lookAt; }

    void lookAt(glm::vec3 lookAt) {
        setCameraView(m_eye, lookAt, m_upVector, m_projectionMatrix);
    }

    glm::mat4x4 getViewMatrix() const { return m_viewMatrix; }
    glm::mat4x4 getProjectionMatrix() const { return m_projectionMatrix; }
    glm::vec3 getUpVector() const { return m_upVector; }

    // Read-only property: fov_and_screen
    glm::vec3 getFovAndScreen() const { return m_fovAndScreen; }

    // Property: fov
    float getFov() const { return m_fovAndScreen.x; }
    void setFov(float fov) { m_fovAndScreen.x = fov; zoom(0.f);/* TODO here update after set fov... not really smart */ }
    
    void setScreenSize(float width, float height) { 
        m_fovAndScreen.y = width; 
        m_fovAndScreen.z = height; 
        setCameraView(m_eye, m_lookAt, m_upVector, m_fovAndScreen);
    }

    /**
     * @brief Returns the view direction of the camera. 
     * View direction is the negative z-axis of the view matrix.
     * @note Exposed
     */
    glm::vec3 getViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }
    glm::vec3 getRightVector() const { return glm::transpose(m_viewMatrix)[0]; }

protected:
	std::string m_name;
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