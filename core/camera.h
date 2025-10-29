#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#include "../include/json.hpp"
using json = nlohmann::json;

#include <string>
#include <algorithm>

struct Camera {



    Camera() = default;

    // TODO Remove this constructor, it is useless, just use default + setCameraView to place the camera
    Camera(std::string name, glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up, glm::vec2 screen) : 
		m_name(name),
        m_eye(std::move(eye)),
        m_lookAt(std::move(lookAt)),
        m_upVector(std::move(up)),
        _screen(std::move(screen)),
        m_projectionMatrix()
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

    virtual glm::mat4 getProjection() const = 0;

    void updateViewMatrix()
    {
        m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);
    }

    virtual void move(glm::vec2 viewportDims, glm::vec2 mousePos, glm::vec2 lastMousePos) = 0;
    virtual void moveRight(float speed) = 0;
    virtual void moveForward(float speed) = 0;
    virtual void moveUp(float speed) = 0;

    virtual void zoom(float delta) = 0;
    virtual void resetZoom() = 0;


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
        // setCameraView(m_eye, lookAt, m_upVector, m_projectionMatrix);
        m_lookAt = lookAt;
        updateViewMatrix();
    }

    glm::mat4x4 getViewMatrix() const { return m_viewMatrix; }
    glm::mat4x4 getProjectionMatrix() const { return m_projectionMatrix; }
    glm::vec3 getWorldUpVector() const { return m_upVector; }

    // Read-only property: fov_and_screen
    // TODO delete ! just keep for compatibility
    glm::vec3 getFovAndScreen() const { return glm::vec3(_fov, _screen); }

    // Property: fov
    float getFov() const { return _fov; }
    void setFov(float fov) { _fov = fov; zoom(0.f);/* TODO here update after set fov... not really smart */ }
    
    void setScreenSize(float width, float height) { 
        _screen.x = width; 
        _screen.y = height;

        m_projectionMatrix = getProjection();
        // setCameraView(m_eye, m_lookAt, m_upVector, getProjection());
    }

    glm::vec3 getRightVector() const { return glm::transpose(m_viewMatrix)[0]; }
    glm::vec3 getUpVector() const { return glm::transpose(m_viewMatrix)[1]; }
    glm::vec3 getViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }

    void saveState(json &j) {
        j["name"] = m_name;
        j["fov"] = _fov;
        j["screen"] = json::array({_screen.x, _screen.y});
        j["eye"] = json::array({m_eye.x, m_eye.y, m_eye.z});
        j["look_at"] = json::array({m_lookAt.x, m_lookAt.y, m_lookAt.z});
        j["up"] = json::array({m_upVector.x, m_upVector.y, m_upVector.z});
        j["type"] = getType();
        doSaveState(j);
    }

    void loadState(json &j) {
        m_name = j["name"].get<std::string>();
        _fov = j["fov"];
        auto &jScreen = j["screen"];
        _screen = glm::vec2(jScreen[0], jScreen[1]);
        auto &jEye = j["eye"];
        m_eye = glm::vec3(jEye[0], jEye[1], jEye[2]);
        auto &jLookAt = j["look_at"];
        m_lookAt = glm::vec3(jLookAt[0], jLookAt[1], jLookAt[2]);
        auto &jUp = j["up"];
        m_upVector = glm::vec3(jUp[0], jUp[1], jUp[2]);
        doLoadState(j);
        // TODO seems not necessary
        m_projectionMatrix = getProjection();
        // setCameraView(m_eye, m_lookAt, m_upVector, getProjection());
    }

    virtual void doSaveState(json &j) = 0;
    virtual void doLoadState(json &j) = 0;

    virtual std::string getType() = 0;

protected:
	std::string m_name;
    float _fov = 45.f;
    glm::vec2 _screen;
    // glm::vec3 m_fovAndScreen;
    glm::mat4x4 m_viewMatrix;
    glm::mat4x4 m_projectionMatrix;
    glm::vec3 m_eye; // Camera position in 3D
    glm::vec3 m_lookAt; // Point that the camera is looking at
    glm::vec3 m_upVector; // Orientation of the camera
    bool m_lock = false;

	float farPlane = 100000.f;
	float nearPlane = 0.1f;

    /**
     * @brief Sigmoid function for smooth zooming.
     */
    float sigmoid(float x, float center=45.f, float w = 90.f, float max_value = 1.f) {
        return (1.f / (1.f + std::exp(-(x - center) / w))) * max_value * 2.;
    }

};