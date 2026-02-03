#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#include "../include/json.hpp"
using json = nlohmann::json;

#include <string>
#include <algorithm>
#include <iostream>

struct Camera {

	Camera (const Camera&) = delete;
	Camera& operator= (const Camera&) = delete;

    Camera() = default;

    Camera(std::string name) : 
		m_name(name),
        m_eye(0.f, 0.f, -3.f),
        m_lookAt(0.f, 0.f, 0.f),
        m_projectionMatrix()
    {
        updateViewMatrix();
    }

	const std::string& getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }

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

    // TODO remove from camera move into specific camera if needed
    void updateViewMatrix()
    {
        m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);
    }

    virtual void lookAtBox(std::tuple<glm::vec3, glm::vec3> box) = 0;
    virtual void move(glm::vec2 oldPos, glm::vec2 newPos) = 0;
    virtual void moveRight(float speed) = 0;
    virtual void moveForward(float speed) = 0;
    virtual void moveUp(float speed) = 0;

    virtual void zoom(float delta) = 0;
    virtual void resetZoom() = 0;


    void setLock(bool lock) { m_lock = lock;}
    bool isLocked() { return m_lock; }

    float getNearPlane() { return nearPlane; }
    void setNearPlane(float val) { nearPlane = val; }

    float getFarPlane() { return farPlane; }
    void setFarPlane(float val) { farPlane = val; }


    glm::vec3 getEye() const { return m_eye; }
    // TODO maybe make private
    void setEye(glm::vec3 eye) { m_eye = std::move(eye); updateViewMatrix(); }

    glm::vec3 getLookAt() const { return m_lookAt; }

    // TODO maybe make private
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
    
    void updateScreenSize(float width, float height) {
        _screen = {width, height}; 
        updateProjectionMatrix();
    }

    glm::vec3 getRightVector() const { return glm::transpose(m_viewMatrix)[0]; }
    glm::vec3 getUpVector() const { return glm::transpose(m_viewMatrix)[1]; }
    glm::vec3 getViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }

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
        m_viewMatrix = glm::mat4(
            jView[0].get<float>(), jView[1].get<float>(), jView[2].get<float>(), jView[3].get<float>(),
            jView[4].get<float>(), jView[5].get<float>(), jView[6].get<float>(), jView[7].get<float>(),
            jView[8].get<float>(), jView[9].get<float>(), jView[10].get<float>(), jView[11].get<float>(),
            jView[12].get<float>(), jView[13].get<float>(), jView[14].get<float>(), jView[15].get<float>()
        );

        auto &jProj = j["proj"];
        m_projectionMatrix = glm::mat4(
            jProj[0].get<float>(), jProj[1].get<float>(), jProj[2].get<float>(), jProj[3].get<float>(),
            jProj[4].get<float>(), jProj[5].get<float>(), jProj[6].get<float>(), jProj[7].get<float>(),
            jProj[8].get<float>(), jProj[9].get<float>(), jProj[10].get<float>(), jProj[11].get<float>(),
            jProj[12].get<float>(), jProj[13].get<float>(), jProj[14].get<float>(), jProj[15].get<float>()
        );

        // Extract position from view matrix
        glm::mat4 c = glm::inverse(m_viewMatrix);
        m_eye = glm::vec3(c[3]);
        // Extract look at from view matrix
        // auto &jLookAt = j["look_at"];
        // m_lookAt = glm::vec3(jLookAt[0], jLookAt[1], jLookAt[2]);
        m_lookAt = {m_viewMatrix[0][2], m_viewMatrix[1][2], m_viewMatrix[2][2]};

        doLoadState(j);
    }

    // TODO here use box of copied camera 
    void copy(Camera &c, std::tuple<glm::vec3, glm::vec3> box) {
        lookAtBox(box);
        setEye(c.getEye());
        setZoom(c.getZoom());
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

	float farPlane = 100.f;
	float nearPlane = 0.1f;

    /**
     * @brief Sigmoid function for smooth zooming.
     */
    float sigmoid(float x, float center=45.f, float w = 90.f, float max_value = 1.f) {
        return (1.f / (1.f + std::exp(-(x - center) / w))) * max_value * 2.;
    }

};