#pragma once
#include "camera.h"

struct TrackBallCamera : public Camera {

    using Camera::Camera;

    glm::vec4 getBounds() {
        float zoomFactor = _zoomFactor + 0.00001f; // Add eps to avoid screen size = 0 at 100%

        auto [min, max] = _box;
        auto wh = max - min;
        auto half = wh / 2.f;

        auto c = (min + max) * .5f;

        float bound = wh.x > wh.y ? half.x : half.y;

        float aspect = _screen.x / _screen.y;

        return {
            -bound * aspect * zoomFactor,
            bound * aspect * zoomFactor,
            -bound * zoomFactor,
            bound * zoomFactor
        };
    }

    void updateProjectionMatrix() override {
        auto b = getBounds();
        m_projectionMatrix = glm::ortho(b.x, b.y, b.z, b.w, nearPlane, farPlane);
    }

    void lookAtBox(std::tuple<glm::vec3, glm::vec3> box) override {
        _zoomFactor = 1.f;

        auto [min, max] = box;
        auto c = (min + max) * .5f;

        // Setup view matrix
        m_eye = {c.x, c.y, c.z + glm::length(max - min)};
        m_lookAt = c;
        m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);

        // m_eye = {0,0,glm::length(max - min)};
        // m_lookAt = {0,0,-1};

        _box = box;

        // Setup proj matrix
        updateProjectionMatrix();
    }

    glm::vec3 mouseToSphere(glm::vec2 p) {
        // Screen coords to NDC
        glm::vec2 v{p.x / _screen.x * 2.f - 1.f, -(p.y / _screen.y * 2.f - 1.f)};
        // v = -v / 1.96f; // Division make the sphere radius greater than 1
        // Division make the sphere radius greater than 1 therefore the border of the sphere is out of screen and this enable to not drag out of the sphere
        v = -v / 2.f;

        // Compute magnitude of v (dist² to the center)
        float mag = glm::dot(v, v);
        glm::vec3 p3{v.x, v.y, 0.};

        if (mag > 1.0) {
            p3 = glm::normalize(p3);
        } else {
            p3 = {p3.x, p3.y, -sqrt(1.0 - mag)};
        }

        return p3;
    }

    void move(glm::vec2 oldPos, glm::vec2 newPos) override {
        if (m_lock)
            return;

        // Compute 3D pos of 2D point on sphere
        glm::vec3 v0 = mouseToSphere(oldPos);
        glm::vec3 v1 = mouseToSphere(newPos);
        // Compute axis of rotation from 3D points
        glm::vec3 ax = glm::cross(v0, v1);

        // Check length to avoid normalize issues (division by 0 can occurs)
        if (glm::length(ax) <= 0.00001f)
            return;

        // Compute angle between the two points on sphere
        float angle = acos(glm::clamp(glm::dot(v0, v1), -1.f, 1.f)) * 1.5f /* speed */;

        // Create quaternion from axis, angle for rotation
        auto q = glm::angleAxis(angle, glm::normalize(ax));
        
        // Translate view to origin for pivot
        auto [min, max] = _box;
        auto c = (min + max) * .5f;

        m_viewMatrix = glm::translate(m_viewMatrix, c);

        // Rotate view
        m_viewMatrix[0] = q * m_viewMatrix[0];
        m_viewMatrix[1] = q * m_viewMatrix[1];
        m_viewMatrix[2] = q * m_viewMatrix[2];

        // Translate view back
        m_viewMatrix = glm::translate(m_viewMatrix, -c);


        // Just update to know where is the camera
        // glm::vec4 position(m_eye.x, m_eye.y, m_eye.z, 1);
        // glm::vec4 pivot(m_lookAt.x, m_lookAt.y, m_lookAt.z, 1);
        // position = (q * (position - pivot)) + pivot;
        // m_eye = position;

        auto cameraMatrix = glm::inverse(m_viewMatrix);
        auto camPos = cameraMatrix[3];
        
        // std::cout << "eye: " << m_eye.x << ", " << m_eye.y << ", " << m_eye.z <<  std::endl;
        // std::cout << "cam pos: " << camPos.x << ", " << camPos.y << ", " << camPos.z << ", " << camPos.w << std::endl;

        m_eye = camPos;

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

        m_viewMatrix = glm::translate(m_viewMatrix, right * offset.x + up * -offset.y);

        m_eye = glm::inverse(m_viewMatrix)[3];
    }

    void moveRight(float speed) override {
        movePan({speed * 500.f, 0});
    }

    void moveForward(float speed) override {
        zoom(speed * 10.f);
    }

    void moveUp(float speed) override {
        movePan({0, speed * 500.f});
    }

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

    void doSaveState(json &j) {
        auto [min, max] = _box;
        j["box"] = json::array({min.x, min.y, min.z, max.x, max.y, max.z});
    }

    void doLoadState(json &j) {
        auto &jBox = j["box"];

        _box = std::make_tuple(
            glm::vec3{jBox[0], jBox[1], jBox[2]},
            glm::vec3{jBox[3], jBox[4], jBox[5]}
        );
    }

    std::string getType() override { return "TrackBallCamera"; }

    private:

    std::tuple<glm::vec3, glm::vec3> _box; // Targeted bounding box
};