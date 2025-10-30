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
    Camera(std::string name, glm::vec3 up) : 
		m_name(name),
        m_eye(0.f, 0.f, -3.f),
        m_lookAt(0.f, 0.f, 0.f),
        // m_upVector(std::move(up)),
        m_projectionMatrix()
    {
        updateViewMatrix();
    }

    

    void setCameraView(glm::vec3 eye, glm::vec3 lookAt, glm::vec3 up)
    {
        m_eye = std::move(eye);
        m_lookAt = std::move(lookAt);
        // m_upVector = std::move(up);
        updateViewMatrix();
    }

    void setCameraProjection(glm::mat4 proj) {
        m_projectionMatrix = std::move(proj);
    }

    virtual glm::mat4 computeProjection() = 0;

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


    void setLock(bool lock) { m_lock = lock;}
    bool isLocked() { return m_lock; }

	const std::string& getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }

    glm::vec3 getEye() const { return m_eye; }
    void setEye(glm::vec3 eye) { m_eye = std::move(eye); updateViewMatrix(); }

    glm::vec3 getLookAt() const { return m_lookAt; }

    void lookAt(glm::vec3 lookAt) {
        m_lookAt = lookAt;
        updateViewMatrix();
        // setCameraView(m_eye, lookAt, m_upVector);
    }

    glm::mat4x4 getViewMatrix() const { return m_viewMatrix; }
    glm::mat4x4 getProjectionMatrix() const { return m_projectionMatrix; }
    glm::vec3 getWorldUpVector() const { return m_upVector; }

    // Property: fov
    float getFov() const { return _fov; }
    
    void setFov(float fov) { 
        _fov = fov; 
        m_projectionMatrix = computeProjection();
    }
    
    void setScreenSize(float width, float height) {
        _screen = {width, height}; 
        m_projectionMatrix = computeProjection();
    }

    glm::vec3 getRightVector() const { return glm::transpose(m_viewMatrix)[0]; }
    glm::vec3 getUpVector() const { return glm::transpose(m_viewMatrix)[1]; }
    glm::vec3 getViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }

    void saveState(json &j) {
        j["name"] = m_name;
        j["fov"] = _fov;
        j["eye"] = json::array({m_eye.x, m_eye.y, m_eye.z});
        j["look_at"] = json::array({m_lookAt.x, m_lookAt.y, m_lookAt.z});
        // j["up"] = json::array({m_upVector.x, m_upVector.y, m_upVector.z});
        j["type"] = getType();
        doSaveState(j);
    }

    void loadState(json &j) {
        m_name = j["name"].get<std::string>();
        _fov = j["fov"];
        auto &jEye = j["eye"];
        m_eye = glm::vec3(jEye[0], jEye[1], jEye[2]);
        auto &jLookAt = j["look_at"];
        m_lookAt = glm::vec3(jLookAt[0], jLookAt[1], jLookAt[2]);
        // auto &jUp = j["up"];
        // m_upVector = glm::vec3(jUp[0], jUp[1], jUp[2]);
        doLoadState(j);

        updateViewMatrix();
    }

    virtual void doSaveState(json &j) = 0;
    virtual void doLoadState(json &j) = 0;

    virtual std::string getType() = 0;

protected:
	std::string m_name;
    // TODO move fov to descent camera
    float _fov = 45.f;
    glm::vec2 _screen;
    glm::mat4x4 m_viewMatrix;
    glm::mat4x4 m_projectionMatrix;
    glm::vec3 m_eye; // Camera position in 3D
    glm::vec3 m_lookAt; // Point that the camera is looking at
    const glm::vec3 m_upVector{0.f, 1.f, 0.f}; // Orientation of the camera
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