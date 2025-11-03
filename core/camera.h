#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#include "../include/json.hpp"
using json = nlohmann::json;

#include <string>
#include <algorithm>

struct Camera {



    Camera() = default;

    Camera(std::string name) : 
		m_name(name),
        m_eye(0.f, 0.f, -3.f),
        m_lookAt(0.f, 0.f, 0.f),
        m_projectionMatrix()
    {
        updateViewMatrix();
    }

    

    void setCameraView(glm::vec3 eye, glm::vec3 lookAt)
    {
        m_eye = std::move(eye);
        m_lookAt = std::move(lookAt);
        updateViewMatrix();
    }

    void setCameraProjection(glm::mat4 proj) {
        m_projectionMatrix = std::move(proj);
    }

    virtual void updateProjectionMatrix() = 0;

    void updateViewMatrix()
    {
        m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);
    }

    virtual void lookAtBox(std::tuple<glm::vec3, glm::vec3> box) = 0;
    virtual void move(glm::vec2 mouseDelta) = 0;
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
    }

    glm::mat4x4 getViewMatrix() const { return m_viewMatrix; }
    glm::mat4x4 getProjectionMatrix() const { return m_projectionMatrix; }
    glm::vec3 getWorldUpVector() const { return m_upVector; }

    float getZoom() const { return 1.f - _zoomFactor; }
    
    void setZoom(float val) { 
        _zoomFactor = 1.f - val;
        updateProjectionMatrix();
    }
    
    void setScreenSize(float width, float height) {
        _screen = {width, height}; 
        updateProjectionMatrix();
    }

    glm::vec3 getRightVector() const { return glm::transpose(m_viewMatrix)[0]; }
    glm::vec3 getUpVector() const { return glm::transpose(m_viewMatrix)[1]; }
    glm::vec3 getViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }

    void saveState(json &j) {
        j["name"] = m_name;
        j["zoom_factor"] = _zoomFactor;
        j["eye"] = json::array({m_eye.x, m_eye.y, m_eye.z});
        j["look_at"] = json::array({m_lookAt.x, m_lookAt.y, m_lookAt.z});
        j["type"] = getType();
        doSaveState(j);
    }

    void loadState(json &j) {
        m_name = j["name"].get<std::string>();
        _zoomFactor = j["zoom_factor"];
        auto &jEye = j["eye"];
        m_eye = glm::vec3(jEye[0], jEye[1], jEye[2]);
        auto &jLookAt = j["look_at"];
        m_lookAt = glm::vec3(jLookAt[0], jLookAt[1], jLookAt[2]);
        doLoadState(j);

        updateViewMatrix();
    }

    void copy(Camera &c, std::tuple<glm::vec3, glm::vec3> box) {
        lookAtBox(box);
        setEye(c.getEye());
        // lookAt(c.getLookAt());
        // setFov(c.getFov());
    }

    virtual void doSaveState(json &j) = 0;
    virtual void doLoadState(json &j) = 0;

    virtual std::string getType() = 0;

protected:
	std::string m_name;
    float _zoomFactor = 1.f;
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

    float dsigmoid_dx(float x, float center = 45.f, float w = 90.f, float max_value = 1.f) {
        float z = std::exp(-(x - center) / w);
        float s = 1.f / (1.f + z);
        return 2.f * max_value * s * (1.f - s) / w;
        // or equivalently:
        // return 2.f * max_value * (z / ((1.f + z)*(1.f + z))) / w;
    }

};