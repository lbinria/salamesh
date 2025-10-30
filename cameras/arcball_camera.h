/*
* Thanks to https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
*/

#pragma once
#include "../core/camera.h"

struct ArcBallCamera : public Camera {

    using Camera::Camera;

    glm::mat4 computeProjection() override {
        float aspect = _screen.x / _screen.y;
        // float b = 0.2f;
        // return glm::ortho(-b, b, -(aspect*b), aspect*b, nearPlane, farPlane);
        // return glm::ortho(-0.305873f, 0.346362f, -0.469889f, 0.530111f, nearPlane, farPlane);

        // return glm::ortho(-0.469889f * aspect, 0.530111f * aspect, -0.469889f, 0.530111f, nearPlane, farPlane);

        // Compute bounds
        _bounds = {
            -0.469889f * aspect * _zoomFactor, 
            0.530111f * aspect * _zoomFactor, 
            -0.469889f * _zoomFactor, 
            0.530111f * _zoomFactor
        };

        return glm::ortho(_bounds.x, _bounds.y, _bounds.z, _bounds.w, nearPlane, farPlane);
    }


    void move(glm::vec2 mouseDelta) override {
        if (m_lock)
            return;

        // Compute delta angles from viewport dimensions
        glm::vec2 delta((2.f * M_PI) / _screen.x, M_PI / _screen.y);
        // Compute actual angles move
        float angleX = -mouseDelta.x * delta.x;
        float angleY = -mouseDelta.y * delta.y;


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
        setCameraView(position, m_lookAt, m_upVector);
    }

    void movePan(glm::vec2 delta) {
        if (m_lock)
            return;

        // Compute view rect size and divide by screen rect size 
        // to get how many world unit per pixel
        glm::vec2 viewDims{_bounds.y - _bounds.x, _bounds.w - _bounds.z};
        glm::vec2 worldUnitPerPixel = viewDims / _screen;

        // Get offset in world coordinates
        glm::vec2 offset = worldUnitPerPixel * delta;

        glm::vec3 right = getRightVector();
        glm::vec3 up = getUpVector();
        // Update camera position / look at
        glm::vec3 nEye = m_eye + right * -offset.x + up * offset.y;
        glm::vec3 nLookAt = m_lookAt + right * -offset.x + up * offset.y;

        // // float dx = delta.x;
        // float dx = delta.x / (2. * tan(0.5 * _fov));

        // float offset = depth / (2. * tan(0.5 * _fov)) * dx;
        // glm::vec3 nEye = m_eye + right * offset;
        // glm::vec3 nLookAt = m_lookAt + right * offset;
        
        setCameraView(nEye, nLookAt, m_upVector);
    }

    void moveRight(float speed) override {
        if (m_lock)
            return;

        m_eye += getRightVector() * speed * 2.f;
        updateViewMatrix();
    }

    void moveForward(float speed) override {}
    void moveUp(float speed) override {}

    void zoom(float delta) {
        float factor = sigmoid(_fov, 45.f, 30.f, 2.5f);
        _zoomFactor += delta * factor;
        // setCameraView(m_eye, m_lookAt, m_upVector, computeProjection());
        m_projectionMatrix = computeProjection();
    }

    void resetZoom() {
        // TODO fill
    }

    void doSaveState(json &j) {}
    void doLoadState(json &j) {}

    std::string getType() override { return "ArcBallCamera"; }

    private:

    float _zoomFactor = 1.f;
    glm::vec4 _bounds;
};