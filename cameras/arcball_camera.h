/*
* Thanks to https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
*/

#pragma once
#include "../core/camera.h"

struct ArcBallCamera : public Camera {

    using Camera::Camera;

    

    glm::vec4 getBounds() {
        float zoomFactor = _zoomFactor + 0.001f; // Add eps to avoid screen size = 0 at 100%

        auto [min, max] = _box;
        auto wh = min - max;

        if (wh.x > wh.y) {
            float aspect = _screen.x / _screen.y;
            
            return {
                min.x * aspect * zoomFactor, 
                max.x * aspect * zoomFactor, 
                min.x * zoomFactor, 
                max.x * zoomFactor
            };
        } else {
            float aspect = _screen.y / _screen.x;
            
            return {
                min.y * zoomFactor, 
                max.y * zoomFactor, 
                min.y * aspect * zoomFactor, 
                max.y * aspect * zoomFactor
            };
        }
    }

    void updateProjectionMatrix() override {
        auto b = getBounds();
        m_projectionMatrix = glm::ortho(b.x, b.y, b.z, b.w, nearPlane, farPlane);
    }

    void lookAtBox(std::tuple<glm::vec3, glm::vec3> box) override {
        _box = box;
        auto [min, max] = box;
        auto c = (max + min) * .5f;
        
        _zoomFactor = 1.f;
        m_eye = {c.x, c.y, c.z + glm::length(max - min)};
        m_lookAt = c;

        updateViewMatrix();
        updateProjectionMatrix();
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
        setCameraView(position, m_lookAt);
    }

    void movePan(glm::vec2 delta) {
        if (m_lock)
            return;

        // Compute view rect size and divide by screen rect size 
        // to get how many world unit per pixel
        auto b = getBounds();
        glm::vec2 viewDims{b.y - b.x, b.w - b.z};
        glm::vec2 worldUnitPerPixel = viewDims / _screen;

        // Get offset in world coordinates
        glm::vec2 offset = worldUnitPerPixel * delta;

        glm::vec3 right = getRightVector();
        glm::vec3 up = getUpVector();
        // Update camera position / look at
        glm::vec3 nEye = m_eye + right * -offset.x + up * offset.y;
        glm::vec3 nLookAt = m_lookAt + right * -offset.x + up * offset.y;
        
        setCameraView(nEye, nLookAt);
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
        // fine-tuned using desmos graph with formula: (1/\ (1+\exp(-(x-c)/w)))*m*2
        // goal is to have greater factor when around _zoomFactor >= 1
        // Change m (max_value) for adjusting speed, but this influences c, w (center, width)
        // Maybe we can found formula to adjust c, w automatically given m
        // or just multiplying delta will be sufficient...
        float factor = sigmoid(_zoomFactor, 0.8f, 0.2f, 0.08f /* factor (max slope of sigmoid) */);
        
        _zoomFactor = std::clamp(_zoomFactor + delta * factor, 0.f, 10.f); 
        updateProjectionMatrix();
    }

    void resetZoom() {
        _zoomFactor = 1.f;
        updateProjectionMatrix();
    }

    void doSaveState(json &j) {}

    void doLoadState(json &j) {}

    std::string getType() override { return "ArcBallCamera"; }

    private:

    glm::vec4 _bounds; // Ortho camera bounds (left, right, bottom, top)
    std::tuple<glm::vec3, glm::vec3> _box; // Targeted bounding box
};