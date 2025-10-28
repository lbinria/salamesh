/*
* Thanks to https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
*/

#pragma once
#include "../core/camera.h"

struct ArcBallCamera : public Camera {

    using Camera::Camera;

    void move(glm::vec2 viewportDims, glm::vec2 mousePos, glm::vec2 lastMousePos) override {
        if (m_lock)
            return;

        // Compute delta angles from viewport dimensions
        glm::vec2 delta((2.f * M_PI) / viewportDims.x, M_PI / viewportDims.y);
        // Compute actual angles move
        float angleX = (lastMousePos.x - mousePos.x) * delta.x;
        float angleY = (lastMousePos.y - mousePos.y) * delta.y;


        // 
        float cosAngle = glm::dot(this->getViewDir(), m_upVector);
        if (cosAngle * glm::sign(angleY) > 0.99f)
            angleY = 0;


        glm::vec4 position(m_eye.x, m_eye.y, m_eye.z, 1);
        glm::vec4 pivot(m_lookAt.x, m_lookAt.y, m_lookAt.z, 1);

        // Rotate camera around pivot point
        glm::mat4 rx(1.f);
        rx = glm::rotate(rx, angleX, m_upVector);
        position = (rx * (position - pivot)) + pivot;

        glm::mat4 ry(1.f);
        ry = glm::rotate(ry, angleY, getRightVector());
        position = (ry * (position - pivot)) + pivot;

        // Update camera
        setCameraView(position, m_lookAt, m_upVector, m_projectionMatrix);
    }

    // void movePan(glm::vec2 viewport, glm::mat4 model, float depth, glm::vec2 oldPos, glm::vec2 newPos) {
    //     if (m_lock)
    //         return;

    //     glm::vec2 delta = newPos - oldPos;

    //     float fov = m_fovAndScreen.x;
    //     float width = m_fovAndScreen.y;
    //     float height = m_fovAndScreen.z;
    //     float aspect = width / height;
    //     float fovY = 2.0f * atanf( tanf(fov * 0.5f) / aspect ); // convert fovX -> fovY

    //     float d = glm::length(m_lookAt - m_eye);
        
    //     glm::vec3 forward = glm::normalize(m_lookAt - m_eye);
    //     glm::vec3 right = glm::normalize(glm::cross(forward, m_upVector));
    //     glm::vec3 camUp = glm::normalize(glm::cross(right, forward));


    //     float worldPerPixelY = (2.0f * d * tanf(fovY * 0.5f)) / height;
    //     float worldPerPixelX = worldPerPixelY * aspect;
        
    //     glm::vec3 offset = right * (-delta.x * worldPerPixelX) + camUp * (delta.y * worldPerPixelY);
        

    //     setCameraView(m_eye + offset, m_lookAt + offset, m_upVector, m_projectionMatrix);
    // }

    void movePan(glm::vec2 viewport, glm::mat4 model, float depth, glm::vec2 oldPos, glm::vec2 newPos) {
        if (m_lock)
            return;

        glm::vec2 delta = newPos - oldPos;

        float d = glm::length(m_lookAt - m_eye);
        
        glm::vec3 forward = glm::normalize(m_lookAt - m_eye);
        glm::vec3 right = glm::normalize(glm::cross(forward, m_upVector));
        glm::vec3 camUp = glm::normalize(glm::cross(right, forward));

        float fov = m_fovAndScreen.x;
        // float worldPerPixelY = (2.0f * d * tan(fov * 0.5f)) / m_fovAndScreen.z;
        // float worldPerPixelX = worldPerPixelY * (m_fovAndScreen.y / m_fovAndScreen.z) /* aspect */;
        // glm::vec3 offset = right * (-delta.x * worldPerPixelX) + camUp * (delta.y * worldPerPixelY);

        float worldPerPixelY = (2.0f * d * tan(fov * 0.5f)) / m_fovAndScreen.z;
        float worldPerPixelX = worldPerPixelY * (m_fovAndScreen.y / m_fovAndScreen.z) /* aspect */;
        glm::vec3 offset = right * (-delta.x * worldPerPixelX) + camUp * (delta.y * worldPerPixelY);
        
        setCameraView(m_eye + offset, m_lookAt + offset, m_upVector, m_projectionMatrix);
    }

    void moveRight(float speed) override {
        if (m_lock)
            return;

        m_eye += getRightVector() * speed * 2.f;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void moveForward(float speed) override {
        if (m_lock)
            return;

        m_eye += m_upVector * speed * 2.f;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);

    }

    void moveUp(float speed) override {
        if (m_lock)
            return;

        m_eye += getViewDir() * speed;
        setCameraView(m_eye, m_lookAt, m_upVector, m_projectionMatrix);
    }

    void doSaveState(json &j) {}
    void doLoadState(json &j) {}

    std::string getType() override { return "ArcBallCamera"; }

};