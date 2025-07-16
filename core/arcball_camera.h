/*
* Thanks to https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
*/

#pragma once
#include "camera.h"

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

    void movePlane(glm::vec2 deltaPos) {
        if (m_lock)
            return;

        glm::vec3 lookAt(m_lookAt.x, m_lookAt.y, m_lookAt.z);
        lookAt -= getRightVector() * deltaPos.x * .005f;
        lookAt += getUpVector() * deltaPos.y * .005f;
        setCameraView(m_eye, lookAt, m_upVector, m_projectionMatrix);
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